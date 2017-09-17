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

Cell::Renderer *renderer;
Cell::FlyCamera camera(math::vec3(0.0f, 1.0f, 0.0f), math::vec3(1.0f, 0.0f, 0.0f));
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
    Log::Message("Initializing GLFW", LOG_INIT);
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        glfwWindowHint(GLFW_SAMPLES, 16);
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

        // register callbacks
        glfwSetFramebufferSizeCallback(window, framebufferSizeFunc);
        glfwSetKeyCallback(window, keyFunc);
        glfwSetCursorPosCallback(window, mousePosFunc);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
    Log::Message("GLFW initialized", LOG_INIT);

    Log::Message("Initializing render system", LOG_INIT);
        renderer = Cell::Init((GLADloadproc)glfwGetProcAddress);
        renderer->SetRenderSize(width, height);
        renderer->SetCamera(&camera);
    Log::Message("Render system initialized", LOG_INIT);

    // configure default OpenGL state
    Log::Message("Configuring OpenGL", LOG_INIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glEnable(GL_MULTISAMPLE);

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    Log::Message("OpenGL configured", LOG_INIT);


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
    //Cell::SceneNode *mainTorus   = Cell::Scene::MakeSceneNode(&torus, matPbr);
    //Cell::SceneNode *secondTorus = Cell::Scene::MakeSceneNode(&torus, matPbr);
    //Cell::SceneNode *thirdTorus  = Cell::Scene::MakeSceneNode(&torus, matPbr);
    //Cell::SceneNode *sphereNode  = Cell::Scene::MakeSceneNode(&sphere, matPbrGlass);

    //mainTorus->AddChild(secondTorus);
    //secondTorus->AddChild(thirdTorus);
    ////thirdTorus->AddChild(sphereNode);

    //mainTorus->Scale    = math::vec3(3.0f);
    //mainTorus->Position = math::vec3(0.0f, 2.5f, 0.0f);
    //secondTorus->Scale  = math::vec3(0.65f);
    //secondTorus->Rotation = math::vec4(0.0, 1.0, 0.0, math::Deg2Rad(90.0));
    //thirdTorus->Scale   = math::vec3(0.65f);
    //sphereNode->Scale   = math::vec3(1.35f);

    Cell::Background background;
    Cell::TextureCube cubemap;
    cubemap.DefaultInitialize(1024, 1024, GL_RGB, GL_UNSIGNED_BYTE);
  
    // - background
    Cell::PBRCapture *pbrEnv = renderer->GetSkypCature();
    background.SetCubemap(pbrEnv->Prefiltered);
	float lodLevel = 1.5f; 
	background.Material->SetFloat("lodLevel", lodLevel);
	float exposure = 1.0;
	background.Material->SetFloat("Exposure", exposure);
    matPbr->GetShader()->Use();
    matPbr->GetShader()->SetFloat("Exposure", exposure);

    // post processing
    Cell::Shader *postProcessing1 = Cell::Resources::LoadShader("postprocessing1", "shaders/screen_quad.vs", "shaders/custom_post_1.fs");
    Cell::Shader *postProcessing2 = Cell::Resources::LoadShader("postprocessing2", "shaders/screen_quad.vs", "shaders/custom_post_2.fs");
    Cell::Material *customPostProcessing1 = renderer->CreatePostProcessingMaterial(postProcessing1);
    Cell::Material *customPostProcessing2 = renderer->CreatePostProcessingMaterial(postProcessing2);

    // test mesh loading
    Cell::SceneNode *test = Cell::Resources::LoadMesh(renderer, "nanosuit", "meshes/sponza/sponza.obj");
    test->Position = math::vec3(0.0, -1.0, 0.0);
    test->Scale = math::vec3(0.01f);

    Cell::DirectionalLight dirLight;
    dirLight.Direction = math::vec3(0.2f, -1.0f, 0.25f);
    dirLight.Color = math::vec3(1.0f, 0.89f, 0.7f);
    dirLight.Intensity = 50.0f;
    renderer->AddLight(&dirLight);

    Cell::DirectionalLight dirLight2;
    dirLight2.Direction = math::vec3(0.5f, -0.9f, 0.0f);
    dirLight2.Color = math::vec3(0.8f, 0.87f, 1.0f);
    dirLight2.Intensity = 25.0f;
    renderer->AddLight(&dirLight2);

    Cell::PointLight light;
    light.Radius = 4.0;
    light.Position = math::vec3(0.0f, 1.0f, 0.0f);
    light.Color = math::vec3(1.0f, 0.25, 0.25f);
    light.Intensity = 50.0f;
    light.RenderMesh = true;
    renderer->AddLight(&light);

    Cell::PointLight light2;
    light2.Radius = 3.0;
    light2.Color = math::vec3(0.5f, 0.5f, 2.0f);
    light2.Intensity = 25.0f;
    light2.RenderMesh = true;
    renderer->AddLight(&light2);

    // bake reflection probes before rendering
    renderer->BakeProbes();

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

            // update render logic
            camera.Update(deltaTime);

            // fill the renderer's command buffer with default test scene
           /* mainTorus->Rotation = math::vec4(math::vec3(1.0f, 0.0f, 0.0f), glfwGetTime());
            secondTorus->Rotation = math::vec4(math::vec3(0.0f, 1.0f, 0.0f), glfwGetTime());
            thirdTorus->Rotation = math::vec4(math::vec3(0.0f, 1.0f, 0.0f), glfwGetTime());
            sphereNode->Rotation = math::vec4(math::normalize(math::vec3(1.0f, 1.0f, 1.0f)), glfwGetTime());*/

            //light.Position = math::vec3(sin(glfwGetTime() * 0.5f) * 10.0, 1.0f, 0.0f);
            light2.Position = math::vec3(sin(glfwGetTime() * 0.3f) * 1.5 + 3.0, 2.0f, cos(glfwGetTime() * 0.1f) * 5.0f);

            dirLight.Direction.x = sin(glfwGetTime() * 0.05f) * 1.5;
            dirLight.Direction.y = -(sin(glfwGetTime() * 0.1f) * 0.5 + 0.5) * 0.5 - 0.5;
            dirLight.Direction.z = (cos(glfwGetTime() * 0.13f) * 0.5 + 0.5) * 0.5 + 0.5;
        }

        {
            //CLOCK(PUSH);
            //renderer->PushRender(mainTorus);
            renderer->PushRender(test);

            renderer->PushRender(&background);

         
        }
        {
            //CLOCK(CUBEMAP);
            // also generate dynamic cubemap from scene
            //renderer.RenderToCubemap(mainTorus, &cubemap, math::vec3(0.0f, 8.0f, 0.0f), 0);
        }

        {
            // push post-processing calls
            //renderer->PushPostProcessor(&customPostProcessing1);
            //renderer->PushPostProcessor(&customPostProcessing2);
        }

        {
            //CLOCK(RENDER);
            // request Cell to render all currently pushed commands
            renderer->RenderPushedCommands();
        }

        glfwSwapBuffers(window);
    }

    // NOTE(Joey): clean up Cell
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
