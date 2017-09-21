uniform int nrProbes;
uniform samplerCube skyIrradiance;
uniform samplerCube skyPrefilter;

uniform samplerCube probe1Irradiance;
uniform samplerCube probe1Prefilter;
uniform samplerCube probe2Irradiance;
uniform samplerCube probe2Prefilter;

uniform vec3 probe1Center;
uniform vec3 probe1AABBMin;
uniform vec3 probe1AABBMax;
uniform vec3 probe2Center;
uniform vec3 probe2AABBMin;
uniform vec3 probe2AABBMax;


float getBoxInfluenceWeight(vec3 localPos, vec3 probeCenter, float innerRange, float outerRange)
{
    // transform from World space to local box (without scaling, so we can test extend box)
    vec3 localPosition = localPos - probeCenter;
    // work in the upper left corner of the box.
    vec3 localDir = vec3(abs(localPosition.x), abs(localPosition.y), abs(localPosition.z));
    localDir = (localDir - innerRange) / (outerRange - innerRange);
    // Take max of all axis
    return max(localDir.x, max(localDir.y, localDir.z));
}

vec3 parallaxCorrectedReflection(vec3 worldPos, vec3 normal, vec3 camPos, vec3 probeCenter, vec3 boxMin, vec3 boxMax)
{
    vec3 V = worldPos - camPos;
    vec3 R = reflect(V, normal);

    // from: https://seblagarde.wordpress.com/2012/09/29/image-based-lighting-approaches-and-parallax-corrected-cubemap/
    // following is the parallax-correction code
    // find the ray intersection with box plane
    vec3 firstPlaneIntersect  = (boxMax - worldPos) / R;
    vec3 secondPlaneIntersect = (boxMin - worldPos) / R;
    // get the furthest of these intersections along the ray
    // (ok because x/0 give +inf and -x/0 give –inf )
    vec3 furthestPlane = max(firstPlaneIntersect, secondPlaneIntersect);
    // find the closest far intersection
    float distance = min(min(furthestPlane.x, furthestPlane.y), furthestPlane.z);

    // get the intersection position
    vec3 intersectPosition = worldPos + R * distance;
    // get corrected reflection
    R = intersectPosition - probeCenter;
    
    return R;
}

vec3 getEnvironmentIrradiance(vec3 worldPos, vec3 normal, vec3 camPos)
{
    vec3 correctedR1 = parallaxCorrectedReflection(worldPos, normal, camPos, probe1Center, probe1AABBMin, probe1AABBMax);
    vec3 color1 = texture(probe1Irradiance, correctedR1).rgb;
    vec3 correctedR2 = parallaxCorrectedReflection(worldPos, normal, camPos, probe2Center, probe2AABBMin, probe2AABBMax);
    vec3 color2 = texture(probe2Irradiance, correctedR2).rgb;
    
    // color1 *= getBlendWeight(worldPos, probe1Center, probe1AABBMin, probe1AABBMax);
    // color2 *= getBlendWeight(worldPos, probe2Center, probe2AABBMin, probe2AABBMax);
    vec3 color = color1;
    return color;
}

vec3 getEnvironmentReflection(vec3 worldPos, vec3 normal, vec3 camPos, float mip)
{
    vec3 correctedR1 = parallaxCorrectedReflection(worldPos, normal, camPos, probe1Center, probe1AABBMin, probe1AABBMax);
    vec3 color1 = textureLod(probe1Prefilter, correctedR1, mip).rgb;
    vec3 correctedR2 = parallaxCorrectedReflection(worldPos, normal, camPos, probe2Center, probe2AABBMin, probe2AABBMax);
    vec3 color2 = textureLod(probe2Prefilter, correctedR2, mip).rgb;
    
    // test weighted NDF blend between cubemap probes.
    float sumNDF         = 0.0;
    float invSumNDF      = 0.0;
    float sumBlendFactor = 0.0;
    float ndf1 = getBoxInfluenceWeight(worldPos, probe1Center, 4.0, 14.0);
    float ndf2 = getBoxInfluenceWeight(worldPos, probe2Center, 2.0, 7.0);
    sumNDF += ndf1;
    sumNDF += ndf2;
    invSumNDF += (1.0 - ndf1);
    invSumNDF += (1.0 - ndf2);
    float blendFactor1 = (1.0 - (ndf1 / sumNDF));
    blendFactor1 *= ((1.0 - ndf1) / invSumNDF);
    float blendFactor2 = (1.0 - (ndf2 / sumNDF));
    blendFactor2 *= ((1.0 - ndf2) / invSumNDF);
    sumBlendFactor += blendFactor1;
    sumBlendFactor += blendFactor2;
    
    float invSumBlend = 1.0 / sumBlendFactor;
    blendFactor1 *= invSumBlend;
    blendFactor2 *= invSumBlend;
    
    color1 *= blendFactor1;
    color2 *= blendFactor2;
    
    return color1 + color2;
}