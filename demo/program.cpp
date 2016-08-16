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
#include <utility/logging/log.h>

void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam);

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
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    Log::Message("OpenGL configured");


    // NOTE(Joey): custom test code
    math::mat3 test;
    math::vec3 test2;
    // NOTE(Joey): check if linking Cell static library worked properly
    int linktest = SuperCalcFunc(1337);
    Log::Message("Testing Cell Linkage: " + std::to_string(linktest), LOG_DEBUG);

    Cell::Shader testShader = Cell::Resources::LoadShader("test", "shaders/test.vs", "shaders/test.fs");
    Cell::Quad quad;
    Cell::LineStrip lineStrip(0.5f, 32);
    Cell::Plane plane(16, 16);
    Cell::Circle circle(16,16);
    Cell::Sphere sphere(8, 8);

    Cell::Texture testTexture = Cell::Resources::LoadTexture("test", "textures/checkerboard.png", GL_TEXTURE_2D, GL_RGB);
    testShader.Use();
    testShader.SetInt("testTexture", 0);

    Log::Display();
    Log::Clear();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);


        // TODO(Joey): do we need to pass input to Cell?
        // TODO(Joey): fill the renderer's command buffer with interesting polygons
        // TODO(Joey): call Cell's renderer

        // NOTE(Joey): simple quad test code to run shader tests on
        testShader.Use();
        testTexture.Bind(0);

        glBindVertexArray(quad.m_VAO); // NOTE(Joey): placeholder for now; will be managed by renderer eventually
            glDrawArrays(GL_TRIANGLE_STRIP, 0, quad.Positions.size());
        glBindVertexArray(0);

        glBindVertexArray(lineStrip.m_VAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, lineStrip.Positions.size());
        glBindVertexArray(0);

        glBindVertexArray(plane.m_VAO);
            glDrawElements(GL_TRIANGLE_STRIP, plane.Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(circle.m_VAO);
            glDrawElements(GL_TRIANGLE_STRIP, circle.Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glBindVertexArray(sphere.m_VAO);
            //glDrawArrays(GL_POINTS, 0, sphere.Positions.size());
            glDrawElements(GL_TRIANGLES, sphere.Indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // NOTE(Joey): display log messages / diagnostics
        Log::Display();
        Log::Clear();

        glfwSwapBuffers(window);
    }

    // TODO(Joey): clean up Cell

    glfwTerminate();

    return 0;
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