#include <iostream>

#include <math/math.h>
#include <utility/logging/log.h>
#include <cell/Cell.h>


#include "scenes/pbr_test.h"

#include <utility/timing/diagnostics.h>

#include <GLFW/glfw3.h>




void framebufferSizeFunc(GLFWwindow *window, int width, int height);
void keyFunc(GLFWwindow *window, int key, int scancode, int action, int mods);
void mousePosFunc(GLFWwindow *window, double xpos, double ypos);

Cell::FlyCamera camera(math::vec3(0.0f, 0.0f, 5.0f), math::vec3(0.0f, 0.0f, -1.0f));
float deltaTime     = 0.0f;
float lastFrameTime = 0.0f;
bool keysPressed[1024];
bool wireframe = false;

/* NOTE(Joey):

  This is a demo program set up to demonstrate the graphic fidelity
  of Cell and demonstrates one way of configuring and using Cell
  from a developer's perspective.

  The demo program also uses Dear IMGUI to add a basic layer of 
  user input interaction.

*/
int main(int argc, char *argv[])
{
    /* NOTE(Joey): 

      Initialize the windowing library.

      Cell is a graphics engine independent of a windowing system. 
      This is to keep Cell cross-platform and independent of any
      windowing system developers prefer to use. Cell does expect
      an OpenGL context set up before initializing Cell.

      Here we use GLFW(3) to initialize a window, initialize an
      OpenGL context and in this case also initialize all OpenGL
      function pointers. Note that loading function pointers is
      something we want Cell to manage in the future as well.

    */
    Log::Message("Initializing GLFW");
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_RESIZABLE, true);
    
        GLFWwindow *window = glfwCreateWindow(1280, 720, "Cell", nullptr, nullptr);           
        if (window == nullptr)
        {
            // TODO(Joey): logging/diagnostics
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

        // NOTE(Joey): register callbacks
        glfwSetFramebufferSizeCallback(window, framebufferSizeFunc);
        glfwSetKeyCallback(window, keyFunc);
        glfwSetCursorPosCallback(window, mousePosFunc);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
    Log::Message("GLFW initialized");

    Log::Message("Initializing render system");
        Cell::Renderer *renderer = Cell::Init((GLADloadproc)glfwGetProcAddress);
        renderer->SetRenderSize(width, height);
        renderer->SetCamera(&camera);
    Log::Message("Render system initialized");

    // NOTE(Joey): configure default OpenGL state
    Log::Message("Configuring OpenGL");
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glEnable(GL_MULTISAMPLE);

        glViewport(0, 0, width, height);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    Log::Message("OpenGL configured");

    Log::Display();
    Log::Clear();


    // NOTE(Joey): shapes
    Cell::Plane plane(16, 16);
    Cell::Sphere sphere(64, 64);
    Cell::Torus torus(2.0f, 0.4f, 32, 32);
    Cell::Cube cube;

    // NOTE(Joey): material setup
    Cell::Material matPbr = renderer->CreateMaterial();
    matPbr.SetTexture("TexAlbedo", Cell::Resources::LoadTexture("rusted metal albedo", "textures/pbr/rusted metal/albedo.png"), 3);
    matPbr.SetTexture("TexNormal", Cell::Resources::LoadTexture("rusted metal normal", "textures/pbr/rusted metal/normal.png"), 4);
    matPbr.SetTexture("TexMetallic", Cell::Resources::LoadTexture("rusted metal metallic", "textures/pbr/rusted metal/metallic.png"), 5);
    matPbr.SetTexture("TexRoughness", Cell::Resources::LoadTexture("rusted metal roughness", "textures/pbr/rusted metal/roughness.png"), 6);
    matPbr.SetTexture("TexAO", Cell::Resources::LoadTexture("rusted metal ao", "textures/pbr/rusted metal/ao.png"), 7);
    Cell::Material matPbrPink = renderer->CreateMaterial();
    matPbrPink.SetTexture("TexAlbedo",    Cell::Resources::LoadTexture("plastic albedo",    "textures/pbr/plastic/albedo.png"),    3);
    matPbrPink.SetTexture("TexNormal",    Cell::Resources::LoadTexture("plastic normal",    "textures/pbr/plastic/normal.png"),    4);
    matPbrPink.SetTexture("TexMetallic",  Cell::Resources::LoadTexture("plastic metallic",  "textures/pbr/plastic/metallic.png"),  5);
    matPbrPink.SetTexture("TexRoughness", Cell::Resources::LoadTexture("plastic roughness", "textures/pbr/plastic/roughness.png"), 6);
    matPbrPink.SetTexture("TexAO",        Cell::Resources::LoadTexture("plastic ao",        "textures/pbr/plastic/ao.png"),        7);
    Cell::Material matPbrGlass = renderer->CreateMaterial("glass");
 
    // NOTE(Joey): configure camera
    camera.SetPerspective(math::Deg2Rad(60.0f), renderer->GetRenderSize().x / renderer->GetRenderSize().y ,0.1f, 100.0f);

    // NOTE(Joey): scene setup
    Cell::SceneNode *mainTorus   = Cell::Scene::MakeSceneNode(&torus, &matPbr);
    Cell::SceneNode *secondTorus = Cell::Scene::MakeSceneNode(&torus, &matPbr);
    Cell::SceneNode *thirdTorus  = Cell::Scene::MakeSceneNode(&torus, &matPbr);
    Cell::SceneNode *sphereNode  = Cell::Scene::MakeSceneNode(&sphere, &matPbrGlass);

    mainTorus->AddChild(secondTorus);
    secondTorus->AddChild(thirdTorus);
    thirdTorus->AddChild(sphereNode);

    mainTorus->Scale    = math::vec3(2.0f);
    mainTorus->Position = math::vec3(0.0f, 2.5f, 0.0f);
    secondTorus->Scale  = math::vec3(0.65f);
    thirdTorus->Scale   = math::vec3(0.65f);
    sphereNode->Scale   = math::vec3(1.35f);

    Cell::SceneNode *floor = Cell::Scene::MakeSceneNode(&plane, &matPbr);
    floor->Rotation        = math::vec4(1.0f, 0.0f, 0.0f, math::Deg2Rad(-90.0f));
    floor->Scale           = math::vec3(10.0f);
    floor->Position        = math::vec3(0.0f, -2.0f, 0.0f);

    Cell::SceneNode *pbrBall = Cell::Scene::MakeSceneNode(&sphere, &matPbrPink);
    pbrBall->Position = math::vec3(5.0f, 5.0f, 4.0f);

    Cell::Background background;
    Cell::TextureCube cubemap;
    cubemap.DefaultInitialize(1024, 1024, GL_RGB, GL_UNSIGNED_BYTE);
   

    // NOTE(Joey): pbr pre-compute
    // TODO(Joey): think of a way we can have a default pre-computed shader set that works at start, without
    // having to require the developer to pre-compute one first; or use build paths and only use IBL if a 
    // cubemap is supplied.   
    Cell::Shader *hdrToCubemap = Cell::Resources::LoadShader("hdr to cubemap", "shaders/cube_sample.vs", "shaders/spherical_to_cube.fs");
    Cell::Shader *irradianceCapture = Cell::Resources::LoadShader("irradiance", "shaders/cube_sample.vs", "shaders/irradiance_capture.fs");
    Cell::Shader *prefilterCapture = Cell::Resources::LoadShader("prefilter", "shaders/cube_sample.vs", "shaders/prefilter_capture.fs");
    Cell::Shader *integrateBrdf = Cell::Resources::LoadShader("integrate_brdf", "shaders/screen_quad.vs", "shaders/integrate_brdf.fs");
    Cell::Material matHDRToCube = renderer->CreateCustomMaterial(hdrToCubemap);
    Cell::Material matIrradianceCapture = renderer->CreateCustomMaterial(irradianceCapture);
    Cell::Material matPrefilterCapture = renderer->CreateCustomMaterial(prefilterCapture);
    Cell::Material matIntegrateBrdf = renderer->CreateCustomMaterial(integrateBrdf);
    matHDRToCube.DepthCompare = GL_LEQUAL;
    matIrradianceCapture.DepthCompare = GL_LEQUAL;
    matPrefilterCapture.DepthCompare = GL_LEQUAL;

    // - convert HDR radiance image to HDR environment cubemap
    Cell::SceneNode *environmentCube = Cell::Scene::MakeSceneNode(&cube, &matHDRToCube);
	Cell::Texture *hdrMap = Cell::Resources::LoadHDR("hdr factory catwalk", "textures/backgrounds/hamarikyu bridge.hdr"); 
	//Cell::Texture *hdrMap = Cell::Resources::LoadHDR("hdr factory catwalk", "textures/backgrounds/Seascape02_downscaled.hdr"); 
    matHDRToCube.SetTexture("environment", hdrMap, 0);
    Cell::TextureCube hdrEnvMap;
    hdrEnvMap.DefaultInitialize(128, 128, GL_RGB, GL_FLOAT);
    renderer->RenderToCubemap(environmentCube, &hdrEnvMap);
    // - irradiance
    Cell::TextureCube irradianceMap;
    irradianceMap.DefaultInitialize(32, 32, GL_RGB, GL_FLOAT);
    matIrradianceCapture.SetTextureCube("environment", &hdrEnvMap, 0);
    environmentCube->Material = &matIrradianceCapture;
    renderer->RenderToCubemap(environmentCube, &irradianceMap, math::vec3(0.0f), 0);
    // - prefilter 
    Cell::TextureCube prefilterMap;
    prefilterMap.FilterMin = GL_LINEAR_MIPMAP_LINEAR;
    prefilterMap.DefaultInitialize(128, 128, GL_RGB, GL_FLOAT, true);
    matPrefilterCapture.SetTextureCube("environment", &hdrEnvMap, 0);
    environmentCube->Material = &matPrefilterCapture;
    // calculate prefilter for multiple roughness levels
    unsigned int maxMipLevels = 5;
    for (unsigned int i = 0; i < maxMipLevels; ++i)
    {
        matPrefilterCapture.SetFloat("roughness", (float)i / (float)(maxMipLevels - 1));
        renderer->RenderToCubemap(environmentCube, &prefilterMap, math::vec3(0.0f), i);

    }
    // - brdf integration
    Cell::RenderTarget brdfTarget(128, 128, GL_HALF_FLOAT, 1, true);
    renderer->Blit(nullptr, &brdfTarget, &matIntegrateBrdf);

    // NOTE(Joey): use pre-computed PBR environment data
    // - pbr shader
    matPbr.SetTextureCube("EnvIrradiance", &irradianceMap, 0);
    matPbr.SetTextureCube("EnvPrefilter", &prefilterMap, 1);
    matPbr.SetTexture("BRDFLUT", brdfTarget.GetColorTexture(0), 2);
    matPbrGlass.SetTextureCube("EnvIrradiance", &irradianceMap, 0);
    matPbrGlass.SetTextureCube("EnvPrefilter", &prefilterMap, 1);
    matPbrGlass.SetTexture("BRDFLUT", brdfTarget.GetColorTexture(0), 2);
    // - background
    background.SetCubemap(&prefilterMap);
	float lodLevel = 1.5f; 
	background.Material->SetFloat("lodLevel", lodLevel);
	float exposure = 1.0;
	background.Material->SetFloat("Exposure", exposure);
    matPbr.SetFloat("Exposure", exposure);
    matPbrGlass.SetFloat("Exposure", exposure);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrameTime = glfwGetTime();
        deltaTime     = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        {
            //CLOCK(UPDATE);
            // TODO(Joey): do we need to pass input to Cell?
            // TODO(Joey): replace by input manager that maps any window input to a 
            // custom defined format; don't think we want this, we'd probably still
            // want to control the camera somewhat from outside the renderer; same
            // with changing material parameters.
            if (keysPressed[GLFW_KEY_W] || keysPressed[GLFW_KEY_UP])
                camera.InputKey(deltaTime, Cell::CAMERA_FORWARD);
            if (keysPressed[GLFW_KEY_S] || keysPressed[GLFW_KEY_DOWN])
                camera.InputKey(deltaTime, Cell::CAMERA_BACK);
            if (keysPressed[GLFW_KEY_A] || keysPressed[GLFW_KEY_LEFT])
                camera.InputKey(deltaTime, Cell::CAMERA_LEFT);
            if (keysPressed[GLFW_KEY_D] || keysPressed[GLFW_KEY_RIGHT])
                camera.InputKey(deltaTime, Cell::CAMERA_RIGHT);
            if (keysPressed[GLFW_KEY_E])
                camera.InputKey(deltaTime, Cell::CAMERA_UP);
            if (keysPressed[GLFW_KEY_Q])
                camera.InputKey(deltaTime, Cell::CAMERA_DOWN);
            if (keysPressed[GLFW_KEY_T])
            {
                lodLevel += 1.0 * deltaTime;
				background.Material->SetFloat("lodLevel", lodLevel);
                Log::Message("LOD:" + std::to_string(lodLevel));
            }
            if (keysPressed[GLFW_KEY_G])
            {
                lodLevel -= 1.0 * deltaTime;
				background.Material->SetFloat("lodLevel", lodLevel);
                Log::Message("LOD:" + std::to_string(lodLevel));
            }
			if (keysPressed[GLFW_KEY_Y])
			{
				exposure += 1.0 * deltaTime;
				background.Material->SetFloat("Exposure", exposure);
                matPbr.SetFloat("Exposure", exposure);
				Log::Message("EXPOSURE:" + std::to_string(exposure));
			}
			if (keysPressed[GLFW_KEY_H])
			{
				exposure -= 1.0 * deltaTime;
				background.Material->SetFloat("Exposure", exposure);
				matPbr.SetFloat("Exposure", exposure);
                Log::Message("EXPOSURE:" + std::to_string(exposure));
			}
            if (keysPressed[GLFW_KEY_Z]) {
                wireframe = !wireframe;
                glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
            }

            // NOTE(Joey): update render logic
            camera.Update(deltaTime);

            // NOTE(Joey): fill the renderer's command buffer with default test scene
            mainTorus->Rotation = math::vec4(math::vec3(1.0f, 0.0f, 0.0f), glfwGetTime());
            secondTorus->Rotation = math::vec4(math::vec3(0.0f, 1.0f, 0.0f), glfwGetTime());
            thirdTorus->Rotation = math::vec4(math::vec3(0.0f, 1.0f, 0.0f), glfwGetTime());
            sphereNode->Rotation = math::vec4(math::normalize(math::vec3(1.0f, 1.0f, 1.0f)), glfwGetTime());
        }

        {
            //CLOCK(PUSH);
            renderer->PushRender(mainTorus);
            //renderer.PushRender(floor);
            renderer->PushRender(pbrBall);

            renderer->PushRender(&background);

            Cell::PointLight light;
            light.Position = math::vec3(sin(glfwGetTime() * 0.5f) * 10.0, 0.0f, 4.0f);
            light.Color = math::vec3(1.0f, 0.7f, 0.7f);
            renderer->PushLight(&light, true);

            Cell::PointLight light2;
            light2.Position = math::vec3(sin(glfwGetTime() * 0.3f) * 5.5, 0.0f, cos(glfwGetTime() * 0.1f) * 10.0f);
            light2.Color = math::vec3(0.5f, 0.5f, 1.0f);
            renderer->PushLight(&light2, true);
        }
        {
            //CLOCK(CUBEMAP);
            // NOTE(Joey): also generate dynamic cubemap from scene
            //renderer.RenderToCubemap(mainTorus, &cubemap, math::vec3(0.0f, 8.0f, 0.0f), 0);
        }

        {
            //CLOCK(RENDER);
            // NOTE(Joey): request Cell to render all currently pushed commands
            renderer->RenderPushedCommands();
        }
   
        // NOTE(Joey): display log messages / diagnostics
        Log::Display();
        Log::Clear();

        glfwSwapBuffers(window);
    }

    // TODO(Joey): clean up Cell
    Cell::Clean();

    glfwTerminate();

    return 0;
}

void framebufferSizeFunc(GLFWwindow *window, int width, int height)
{
    // TODO(Joey): reset viewport, but remain proper aspect ratio
}

void keyFunc(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keysPressed[key] = true;
        else if (action == GLFW_RELEASE)
            keysPressed[key] = false;
    }
}

bool firstMouse = true;
float lastX = 640.0f;
float lastY = 360.0f;
void mousePosFunc(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.InputMouse(xoffset, yoffset);
}
