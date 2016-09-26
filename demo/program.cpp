#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math/math.h>
#include <cell/ProjectLinkTest.h>
#include <cell/resources/resources.h>
#include <cell/shading/shader.h>
#include <cell/shading/texture.h>
#include <cell/mesh/quad.h>
#include <cell/mesh/plane.h>
#include <cell/mesh/circle.h>
#include <cell/mesh/sphere.h>
#include <cell/mesh/line_strip.h>
#include <cell/mesh/torus.h>
#include <cell/mesh/cube.h>
#include <utility/logging/log.h>
#include <cell/camera/fly_camera.h>
#include <cell/scene/scene.h>
#include <cell/scene/background.h>
#include <cell/renderer/renderer.h>
#include <cell/lighting/point_light.h>
#include <cell/renderer/render_target.h>

#include "scenes/pbr_test.h"

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam);

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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        glfwWindowHint(GLFW_RESIZABLE, true);
    
        GLFWwindow *window = glfwCreateWindow(1280, 720, "Cell", nullptr, nullptr);           
        //std::cout << *ptrToTest << std::endl;
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

    Log::Message("GLFW initialized");

    // TODO(Joey): initialize Cell here
    //Cell::Init();

    // NOTE(Joey): load OpenGL function pointers
    Log::Message("Initializing GLEW");
        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            // TODO(Joey): logging/diagnostics
            return -1;
        }
       glGetError();

    Log::Message("GLEW initialized");

    // TODO(Joey): move to Cell initialization; will by initialized by default if windowing library
    // requests a Debug context; otherwise we simply ignore debug output.
    Log::Message("Initializing debug Output.");
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        // NOTE(Joey): we succesfully requested a debug context, now initialize
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
    }

    Log::Message("Debug output initialized.");


    // NOTE(Joey): configure default OpenGL state
    Log::Message("Configuring OpenGL");
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    Log::Message("OpenGL configured");


    // NOTE(Joey): custom test code
    Cell::Renderer renderer;
    renderer.Init();
    renderer.SetCamera(&camera);

    Cell::Shader *testShader = Cell::Resources::LoadShader("test", "shaders/test.vs", "shaders/test.fs");
    //Cell::Quad quad;
    //Cell::LineStrip lineStrip(0.5f, 32);
    Cell::Plane plane(16, 16);
    //Cell::Circle circle(16,16);
    Cell::Sphere sphere(64, 64);
    Cell::Torus torus(2.0f, 0.4f, 32, 32);

    //Cell::Texture testTexture = Cell::Resources::LoadTexture("test", "textures/checkerboard.png", GL_TEXTURE_2D, GL_RGB);
    Cell::Texture     *testTexture = Cell::Resources::LoadTexture("test", "textures/scuffed plastic/roughness.png", GL_TEXTURE_2D, GL_RGB);
    Cell::TextureCube *cubemap     = Cell::Resources::LoadTextureCube("yokohama night", "textures/backgrounds/yokohama night/");

    Log::Display();
    Log::Clear();

    // NOTE(Joey): configure camera
    camera.SetPerspective(math::Deg2Rad(60.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    // NOTE(Joey): set up default scene w/ materials
    Cell::Material defaultMaterial;
    defaultMaterial.SetShader(testShader);
    defaultMaterial.SetTexture("testTexture", testTexture, 0);

    Cell::SceneNode *mainTorus   = Cell::Scene::MakeSceneNode(&torus, &defaultMaterial);
    Cell::SceneNode *secondTorus = Cell::Scene::MakeSceneNode(&torus, &defaultMaterial);
    Cell::SceneNode *thirdTorus  = Cell::Scene::MakeSceneNode(&torus, &defaultMaterial);
    Cell::SceneNode *sphereNode  = Cell::Scene::MakeSceneNode(&sphere, &defaultMaterial);

    mainTorus->AddChild(secondTorus);
    secondTorus->AddChild(thirdTorus);
    thirdTorus->AddChild(sphereNode);

    mainTorus->Scale    = math::vec3(2.0f);
    mainTorus->Position = math::vec3(0.0f, 2.5f, 0.0f);
    secondTorus->Scale  = math::vec3(0.65f);
    thirdTorus->Scale   = math::vec3(0.65f);
    sphereNode->Scale   = math::vec3(1.35f);

    Cell::SceneNode *floor = Cell::Scene::MakeSceneNode(&plane, &defaultMaterial);
    floor->Rotation        = math::vec4(1.0f, 0.0f, 0.0f, math::Deg2Rad(-90.0f));
    floor->Scale           = math::vec3(10.0f);
    floor->Position        = math::vec3(0.0f, -2.0f, 0.0f);

    Cell::Background background;
    background.SetCubemap(cubemap);

    Cell::RenderTarget target(512, 512, GL_UNSIGNED_BYTE, 2, true);

    defaultMaterial.SetTexture("testTexture", target.GetColorTexture(0), 0);
    //defaultMaterial.SetTexture("testTexture", testTexture, 0);

    // scene management:
    //ScenePbrTest scene(&renderer, &camera);
    //scene.Init();

    Cell::TextureCube cubez;
    cubez.DefaultInitialize(1024, 1024, GL_RGB, GL_UNSIGNED_BYTE);
    //background.SetCubemap(&cubez);
   

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrameTime = glfwGetTime();
        deltaTime     = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // TODO(Joey): do we need to pass input to Cell?
        // TODO(Joey): replace by input manager that maps any window input to a 
        // custom defined format.
        if (keysPressed[GLFW_KEY_W] || keysPressed[GLFW_KEY_UP])
            camera.InputKey(deltaTime, Cell::CAMERA_FORWARD);
        if (keysPressed[GLFW_KEY_S] || keysPressed[GLFW_KEY_DOWN])
            camera.InputKey(deltaTime, Cell::CAMERA_BACK);
        if (keysPressed[GLFW_KEY_A] || keysPressed[GLFW_KEY_LEFT])
            camera.InputKey(deltaTime, Cell::CAMERA_LEFT);
        if (keysPressed[GLFW_KEY_D] || keysPressed[GLFW_KEY_RIGHT])
            camera.InputKey(deltaTime, Cell::CAMERA_RIGHT);

        if (keysPressed[GLFW_KEY_Z]) {
            wireframe = !wireframe;
            glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        }

        // NOTE(Joey): update render logic
        camera.Update(deltaTime);

        // NOTE(Joey): fill the renderer's command buffer with default test scene
        mainTorus->Rotation   = math::vec4(math::vec3(1.0f, 0.0f, 0.0f), glfwGetTime());
        secondTorus->Rotation = math::vec4(math::vec3(0.0f, 1.0f, 0.0f), glfwGetTime());
        thirdTorus->Rotation  = math::vec4(math::vec3(0.0f, 1.0f, 0.0f), glfwGetTime());
        sphereNode->Rotation  = math::vec4(math::normalize(math::vec3(1.0f, 1.0f, 1.0f)), glfwGetTime());

        renderer.PushRender(mainTorus);
        renderer.PushRender(floor);

        background.PushRender(&renderer);

        renderer.SetTarget(&target);
        background.PushRender(&renderer);
        renderer.PushRender(mainTorus);
        renderer.SetTarget(nullptr);

        Cell::PointLight light;
        light.Position = math::vec3(sin(glfwGetTime() * 0.5f) * 10.0, 0.0f, 4.0f);
        light.Color = math::vec3(1.0f, 0.7f, 0.7f);
        renderer.PushLight(&light, true);

        // NOTE(Joey): also generate dynamic cubemap from scene
        renderer.RenderToCubemap(mainTorus, cubemap, 1024, 1024, math::vec3(0.0f, 8.0f, 0.0f), 0);

        // NOTE(Joey): request Cell to render all currently pushed commands
        renderer.RenderPushedCommands();
   
        // NOTE(Joey): display log messages / diagnostics
        Log::Display();
        Log::Clear();

        glfwSwapBuffers(window);
    }

    // TODO(Joey): clean up Cell

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


// NOTE(Joey): process OpenGL's debug output
// TODO(Joey): this should be included in Cell
void APIENTRY glDebugOutput(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar *message,
    void *userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::string logMessage = "Debug output: (" + std::to_string(id) + "): " + message + "\n";

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             logMessage += "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   logMessage += "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: logMessage += "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     logMessage += "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     logMessage += "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           logMessage += "Source: Other"; break;
    } logMessage += "\n";

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               logMessage += "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: logMessage += "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  logMessage += "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         logMessage += "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         logMessage += "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              logMessage += "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          logMessage += "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           logMessage += "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               logMessage += "Type: Other"; break;
    } logMessage += "\n";

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         logMessage += "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       logMessage += "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          logMessage += "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: logMessage += "Severity: notification"; break;
    } logMessage += "\n";
    logMessage += "\n";

    Log::Message(logMessage, type == GL_DEBUG_TYPE_ERROR ? LOG_ERROR : LOG_WARNING);
}