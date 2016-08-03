#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include <math/math.h>
#include <cell/ProjectLinkTest.h>


/* NOTE(Joey):

  This is a demo program set up to demonstrate the graphic fidelity
  of Cell and demonstrates one way of configuring and using Cell
  from a developer's perspective.

  The demo program also uses Dear IMGUI to add a basic layer of 
  user input interaction.

*/
int main(int argc, int *argv[])
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
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, true);
    
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Cell", nullptr, nullptr);
    if (window == nullptr)
    {
        // TODO(Joey): logging/diagnostics
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // TODO(Joey): initialize Cell here

    // NOTE(Joey): load OpenGL function pointers
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        // TODO(Joey): logging/diagnostics
        return -1;
    }

    // NOTE(Joey): configure default OpenGL state
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.4f, 0.1f, 0.1f, 1.0f);


    // NOTE(Joey): custom test code
    math::mat3 test;
    math::vec3 test2;
    // NOTE(Joey): check if linking Cell static library worked properly
    int linktest = SuperCalcFunc(1337);
    std::cout << linktest << std::endl;


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO(Joey): do we need to pass input to Cell?
        // TODO(Joey): fill the renderer's command buffer with interesting polygons
        // TODO(Joey): call Cell's renderer

        glfwSwapBuffers(window);
    }

    // TODO(Joey): clean up Cell

    glfwTerminate();

    return 0;
}