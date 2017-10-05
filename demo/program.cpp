#include <iostream>

#include <math/math.h>
#include <utility/logging/log.h>
#include <cell/Cell.h>


#include "scenes/pbr_test.h"

#include <utility/timing/diagnostics.h>
#include <utility/random/random.h>

#include <GLFW/glfw3.h>

void framebufferSizeFunc(GLFWwindow *window, int width, int height);
void keyFunc(GLFWwindow *window, int key, int scancode, int action, int mods);
void mousePosFunc(GLFWwindow *window, double xpos, double ypos);
void mouseButtonFunc(GLFWwindow*, int button, int action, int mods);
void mouseScrollFunc(GLFWwindow*, double xoffset, double yoffset);

Cell::Renderer *renderer;
Cell::FlyCamera camera(math::vec3(0.0f, 1.0f, 0.0f), math::vec3(0.0f, 0.0f, -1.0f));
float deltaTime     = 0.0f;
float lastFrameTime = 0.0f;
bool keysPressed[1024];
bool keysActive[1024];
bool wireframe = false;
bool renderGUI = false;

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
    Log::Message("Initializing GLFW", LOG_INIT);
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        glfwWindowHint(GLFW_RESIZABLE, true);
    
        GLFWwindow* window = glfwCreateWindow(1920, 1080, "Cell", nullptr, nullptr);           
        if (window == nullptr)
        {
            // TODO(Joey): logging/diagnostics
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

        // register callbacks
        glfwSetFramebufferSizeCallback(window, framebufferSizeFunc);
        glfwSetKeyCallback(window, keyFunc);
        glfwSetCursorPosCallback(window, mousePosFunc);
        glfwSetMouseButtonCallback(window, mouseButtonFunc);
        glfwSetScrollCallback(window, mouseScrollFunc);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
    Log::Message("GLFW initialized", LOG_INIT);

    Log::Message("Initializing render system", LOG_INIT);
        renderer = Cell::Init(window, (GLADloadproc)glfwGetProcAddress);
        renderer->SetRenderSize(width, height);
        renderer->SetCamera(&camera);
    Log::Message("Render system initialized", LOG_INIT);

    // basic shapes
    Cell::Plane plane(16, 16);
    Cell::Sphere sphere(64, 64);
    Cell::Torus torus(2.0f, 0.4f, 32, 32);
    Cell::Cube cube;

    // material setup
    Cell::Material *matPbr = renderer->CreateMaterial();
    Cell::Material *matPbrGlass = renderer->CreateMaterial("glass");
 
    // configure camera
    camera.SetPerspective(math::Deg2Rad(60.0f), renderer->GetRenderSize().x / renderer->GetRenderSize().y ,0.1f, 100.0f);

    // scene setup
    Cell::SceneNode *mainTorus   = Cell::Scene::MakeSceneNode(&torus, matPbr);
    Cell::SceneNode *secondTorus = Cell::Scene::MakeSceneNode(&torus, matPbr);
    Cell::SceneNode *thirdTorus  = Cell::Scene::MakeSceneNode(&torus, matPbr);
    Cell::SceneNode *sphereNode  = Cell::Scene::MakeSceneNode(&sphere, matPbrGlass);

    mainTorus->AddChild(secondTorus);
    secondTorus->AddChild(thirdTorus);
    thirdTorus->AddChild(sphereNode);

    mainTorus->SetScale(1.0f);
    mainTorus->SetPosition(math::vec3(-3.0f, 7.5f, 0.0f));
    secondTorus->SetScale(0.65f);
    secondTorus->SetRotation(math::vec4(0.0, 1.0, 0.0, math::Deg2Rad(90.0)));
    thirdTorus->SetScale(0.65f);
    sphereNode->SetScale(1.35f);

    Cell::Background* background = new Cell::Background;
  
    // - background
    Cell::PBRCapture *pbrEnv = renderer->GetSkypCature();
    background->SetCubemap(pbrEnv->Prefiltered);
	float lodLevel = 1.5f; 
	background->Material->SetFloat("lodLevel", lodLevel);
	float exposure = 1.0;
	background->Material->SetFloat("Exposure", exposure);
    matPbr->GetShader()->Use();
    matPbr->GetShader()->SetFloat("Exposure", exposure);

    // post processing
    Cell::Shader *postProcessing1 = Cell::Resources::LoadShader("postprocessing1", "shaders/screen_quad.vs", "shaders/custom_post_1.fs");
    Cell::Shader *postProcessing2 = Cell::Resources::LoadShader("postprocessing2", "shaders/screen_quad.vs", "shaders/custom_post_2.fs");
    Cell::Material *customPostProcessing1 = renderer->CreatePostProcessingMaterial(postProcessing1);
    Cell::Material *customPostProcessing2 = renderer->CreatePostProcessingMaterial(postProcessing2);

    // mesh 
    Cell::SceneNode* sponza = Cell::Resources::LoadMesh(renderer, "sponza", "meshes/sponza/sponza.obj");
    sponza->SetPosition(math::vec3(0.0, -1.0, 0.0));
    sponza->SetScale(0.01f);
   /* Cell::SceneNode* fellLord = Cell::Resources::LoadMesh(renderer, "felllord", "meshes/fellord/fellord.obj");
    fellLord->SetRotation(math::vec4(0.0f, 1.0f, 0.0f, math::Deg2Rad(110.0f)));
    fellLord->SetPosition(math::vec3(0.0f, -1.0f, 0.0f));*/
 /*   Cell::SceneNode* lamp = Cell::Resources::LoadMesh(renderer, "lamp", "meshes/lamp/lamp.obj");
    lamp->SetPosition(math::vec3(-2.0f, -1.0f, 0.0f));
    lamp->SetScale(5.0f);
    Cell::SceneNode* turretTop = Cell::Resources::LoadMesh(renderer, "turret_top", "meshes/turret/turret_top.obj");
    turretTop->SetPosition(math::vec3(3.0f, -1.0f, 0.0f));
    Cell::SceneNode* turretBottom = Cell::Resources::LoadMesh(renderer, "turret_bottom", "meshes/turret/turret_bottom.obj");
    turretBottom->SetPosition(math::vec3(3.0f, -1.0f, 0.0f));*/

    // lighting
    Cell::DirectionalLight dirLight;
    dirLight.Direction = math::vec3(0.2f, -1.0f, 0.25f);
    dirLight.Color = math::vec3(1.0f, 0.89f, 0.7f);
    dirLight.Intensity = 50.0f;
    renderer->AddLight(&dirLight);

    std::vector<Cell::PointLight> torchLights;
    {
        Cell::PointLight torch;
        torch.Radius = 2.5;
        torch.Color = math::vec3(1.0f, 0.3f, 0.05f);
        torch.Intensity = 50.0f;
        torch.RenderMesh = true;

        torch.Position = math::vec3( 4.85f, 0.7f, 1.43f);
        torchLights.push_back(torch);
        torch.Position = math::vec3( 4.85f, 0.7f, -2.2f);
        torchLights.push_back(torch);
        torch.Position = math::vec3(-6.19f, 0.7f, 1.43f);
        torchLights.push_back(torch);
        torch.Position = math::vec3(-6.19f, 0.7f, -2.2f);
        torchLights.push_back(torch);
        renderer->AddLight(&torchLights[0]);
        renderer->AddLight(&torchLights[1]);
        renderer->AddLight(&torchLights[2]);
        renderer->AddLight(&torchLights[3]);
    }
    
    std::vector<Cell::PointLight> randomLights;
    std::vector<math::vec3> randomLightStartPositions;
    {
        for (int i = 0; i < 100; ++i)
        {
            Cell::PointLight light;
            light.Radius = 1.0 + Random::Uniliteral() * 3.0;
            light.Intensity = 10.0 + Random::Uniliteral() * 1000.0;
            light.Color = math::vec3(Random::Uniliteral(), Random::Uniliteral(), Random::Uniliteral());
            light.RenderMesh = true;
            randomLights.push_back(light);
            randomLightStartPositions.push_back(math::vec3(Random::Biliteral() * 12.0f, Random::Uniliteral() * 5.0f, Random::Biliteral() * 6.0f));
        }
        for (int i = 0; i < randomLights.size(); ++i)
        {
            //renderer->AddLight(&randomLights[i]);
        }
    }

    // bake irradiance GI (with grid placement of probes)
    {
        // bottom floor - center
        renderer->AddIrradianceProbe(math::vec3(  0.0f,  0.5f, -0.5f), 3.25);
        renderer->AddIrradianceProbe(math::vec3(  3.0f,  0.5f, -0.5f), 3.25);
        renderer->AddIrradianceProbe(math::vec3(  6.0f,  0.5f, -0.5f), 3.25);
        renderer->AddIrradianceProbe(math::vec3(  8.5f,  0.5f, -0.5f), 3.25);
        renderer->AddIrradianceProbe(math::vec3( 11.4f,  0.5f, -0.5f), 4.25);
        renderer->AddIrradianceProbe(math::vec3( -3.0f,  0.5f, -0.5f), 3.25);
        renderer->AddIrradianceProbe(math::vec3( -6.2f,  0.5f, -0.5f), 3.25);
        renderer->AddIrradianceProbe(math::vec3( -9.5f,  0.5f, -0.5f), 3.25);
        renderer->AddIrradianceProbe(math::vec3(-12.1f,  0.5f, -0.5f), 4.25);
        // bottom floor - left wing
        renderer->AddIrradianceProbe(math::vec3(  0.0f, 0.5f, 4.0f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(  4.0f, 0.5f, 4.0f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(  8.0f, 0.5f, 4.0f), 4.0);
        renderer->AddIrradianceProbe(math::vec3( 12.0f, 0.5f, 4.0f), 4.0);
        renderer->AddIrradianceProbe(math::vec3( -4.0f, 0.5f, 4.0f), 4.0);
        renderer->AddIrradianceProbe(math::vec3( -8.0f, 0.5f, 4.0f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(-12.0f, 0.5f, 4.0f), 4.0);
        // bottom floor - right wing
        renderer->AddIrradianceProbe(math::vec3(  0.0f, 0.5f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(  4.0f, 0.5f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(  8.0f, 0.5f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3( 12.0f, 0.5f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3( -4.0f, 0.5f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3( -8.0f, 0.5f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(-12.0f, 0.5f, -4.5f), 4.0);
        // 1st floor - center wing
        renderer->AddIrradianceProbe(math::vec3(  0.0f, 5.0f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3(  4.0f, 5.0f, -0.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(  8.0f, 5.0f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3( 12.0f, 5.0f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3( -4.0f, 5.0f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3( -8.0f, 5.0f, -0.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(-12.0f, 5.0f, -0.5f), 4.5);
        // 1st floor - left wing
        renderer->AddIrradianceProbe(math::vec3(  0.0f, 5.0f, 4.0), 4.0);
        renderer->AddIrradianceProbe(math::vec3(  4.0f, 5.0f, 4.0), 4.0);
        renderer->AddIrradianceProbe(math::vec3(  8.0f, 5.0f, 4.0), 4.0);
        renderer->AddIrradianceProbe(math::vec3( 12.0f, 5.0f, 4.0), 4.0);
        renderer->AddIrradianceProbe(math::vec3( -4.0f, 5.0f, 4.0), 4.0);
        renderer->AddIrradianceProbe(math::vec3( -8.0f, 5.0f, 4.0), 4.0);
        renderer->AddIrradianceProbe(math::vec3(-11.5f, 5.0f, 4.0), 4.0);
        // 1st floor - right wing
        renderer->AddIrradianceProbe(math::vec3(  0.0f, 5.0f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(  4.0f, 5.0f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(  8.0f, 5.0f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3( 12.0f, 5.0f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3( -4.0f, 5.0f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3( -8.0f, 5.0f, -4.5f), 4.0);
        renderer->AddIrradianceProbe(math::vec3(-11.5f, 5.0f, -4.5f), 4.0);
        // 2nd floor - center wing
        renderer->AddIrradianceProbe(math::vec3(  0.0f, 9.5f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3(  4.0f, 9.5f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3(  8.0f, 9.5f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3( 12.0f, 9.5f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3( -4.0f, 9.5f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3( -8.0f, 9.5f, -0.5f), 4.5);
        renderer->AddIrradianceProbe(math::vec3(-11.5f, 9.5f, -0.5f), 4.5);

        // bake before rendering
        renderer->BakeProbes();
    }
  

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        Cell::NewFrame();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrameTime = glfwGetTime();
        deltaTime     = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        {
            //CLOCK(UPDATE);
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
            if (keysPressed[GLFW_KEY_TAB] && !keysActive[GLFW_KEY_TAB])
            {
                renderGUI = !renderGUI;
                keysActive[GLFW_KEY_TAB] = true;
            }

            // update render logic
            camera.Update(deltaTime);

            Log::Message("(" + std::to_string(camera.Position.x) + ", " + std::to_string(camera.Position.y) + ", " + std::to_string(camera.Position.z) + ")", LOG_DEBUG);

            // fill the renderer's command buffer with default test scene
            mainTorus->SetRotation(math::vec4(math::vec3(1.0f, 0.0f, 0.0f), glfwGetTime() * 2.0));
            secondTorus->SetRotation(math::vec4(math::vec3(0.0f, 1.0f, 0.0f), glfwGetTime() * 3.0));
            thirdTorus->SetRotation(math::vec4(math::vec3(0.0f, 1.0f, 0.0f), glfwGetTime() * 4.0));
            sphereNode->SetRotation(math::vec4(math::normalize(math::vec3(1.0f, 1.0f, 1.0f)), glfwGetTime()));

            //turretTop->SetRotation(math::vec4(0.0f, 1.0f, 0.0f, glfwGetTime()));

          /*  dirLight.Direction.x = sin(glfwGetTime() * 0.05f) * 1.5;
            dirLight.Direction.y = -(sin(glfwGetTime() * 0.1f) * 0.5 + 0.5) * 0.5 - 0.5;
            dirLight.Direction.z = (cos(glfwGetTime() * 0.13f) * 0.5 + 0.5) * 0.5 + 0.5;*/

            for (int i = 0; i < torchLights.size(); ++i)
            {
                torchLights[i].Radius = 1.5f + 0.1 * std::cos(std::sin(glfwGetTime() * 1.37 + i * 7.31) * 3.1 + i);
                torchLights[i].Intensity = 25.0f + 5.0 * std::cos(std::sin(glfwGetTime() * 0.67 + i * 2.31) * 2.31 * i);
            }

            for (int i = 0; i < randomLights.size(); ++i)
            {
                randomLights[i].Position = randomLightStartPositions[i] + math::vec3(std::sin(glfwGetTime() * 0.31f + i * 3.17f) * 1.79f, std::cos(glfwGetTime() * 0.21f + i * 1.11f) * 1.61f, std::sin(glfwGetTime() * 0.49 + i * 0.79f) * 1.31);
            }
        }

        {
            //CLOCK(PUSH);
            renderer->PushRender(mainTorus);
            renderer->PushRender(sponza);
            //renderer->PushRender(fellLord);
            renderer->PushRender(background);
            //renderer->PushRender(lamp);
            //renderer->PushRender(turretTop);
            //renderer->PushRender(turretBottom);
        }
        {
            // push post-processing calls
            //renderer->PushPostProcessor(customPostProcessing1);
            //renderer->PushPostProcessor(customPostProcessing2);
        }

        {
            //CLOCK(RENDER);
            // request Cell to render all currently pushed commands
            renderer->RenderPushedCommands();
        }

        // GUI
        if (renderGUI)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            Cell::RenderGUI();
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        glfwSwapBuffers(window);
    }

    // clean up Cell
    Cell::Clean();

    glfwTerminate();

    return 0;
}

void framebufferSizeFunc(GLFWwindow *window, int width, int height)
{
    renderer->SetRenderSize(width, height);
    camera.SetPerspective(math::Deg2Rad(60.0f), (float)width / (float)height, 0.1f, 100.0f);
}

void keyFunc(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keysPressed[key] = true;
        else if (action == GLFW_RELEASE)
        {
            keysPressed[key] = false;
            keysActive[key] = false;
        }
    }
    Cell::InputKey(key, action);
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
        float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to left

        lastX = xpos;
        lastY = ypos;
    if (!renderGUI)
    {
        camera.InputMouse(xoffset, yoffset);
    }
}

void mouseButtonFunc(GLFWwindow*, int button, int action, int mods)
{
    Cell::InputMouse(button, action);
}

void mouseScrollFunc(GLFWwindow*, double xoffset, double yoffset)
{
    Cell::InputScroll(yoffset);
}

