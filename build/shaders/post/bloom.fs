#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D HDRScene;

void main()
{
    vec3 hdrColor = texture(HDRScene, TexCoords).rgb;
    // float grayscale = dot(hdrColor, vec3(0.299, 0.587, 0.114));
    // hdrColor = smoothstep(1.0, 4.0, grayscale) * hdrColor;
    
    // FragColor.rgb = hdrColor;
    /* NOTE(Joey):
    
      Usually we render the bloom effect above a certain threshold (like all pixels that have 
      a luminence of >= 1.0 are considered part of the bloom effect). This is physically
      inaccurate as bloom is meant to model inaccuracies of the human eye lens, where some 
      percentange of light is diffused into blurry shapes.
      
      Without an HDR render pipeline it is still advisable to stick to a threshold approach, but
      since we currently render in an HDR render pipeline (where we have a proper tonemapping 
      pass) at the end we can allow all light to pass through the 'fake' lens.
      
      Scene objects averaging a brightness of around 1.0 would contribute only 0.1 in the bloom
      output while a light of intensity 1000.0 now adds 100.0 to the bloom effect, keeping the 
      relative glow we're used to.
      
      Note: I still need to tweak this; not at a level where I'm satisfied yet, 0.1 seems to do 
      to little effect on most scenes.
    
    */
    FragColor.rgb = 0.1 * hdrColor;
    FragColor.a = 1.0;
}