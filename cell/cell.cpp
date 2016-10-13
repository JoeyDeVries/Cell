#include "cell.h"

#include <utility/logging/log.h>

namespace Cell
{
    void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam);

    // NOTE(Joey): hold a pointer to the renderer so we can delete it during 
    // cleanup
    Renderer* renderer;
    Renderer* Init(GLADloadproc loadProcFunc)
    {
        /* NOTE(Joey):

        Before doing anything with OpenGL we have to load all its functions
        into stored function pointers from the OpenGL driver. Windows by
        default only hosts a small set of OpenGL functions in its library
        so all the more 'modern' functions have to be manually loaded/
        requested from the graphics cards' OpenGL driver itself.

        This is a cumbersome work as we generally have to store all the
        required function prototypes, function pointer storage and all
        the enum / unique ids of the OpenGL functionality and on top of
        that we have to query for the function's memory location for
        each and every one of these functions.

        For this reason we use a library called ... (glad?)
        http://glad.dav1d.de/ <- their webservice for generating header file

        */
        if (!gladLoadGLLoader(loadProcFunc))
        {
            Log::Message("Failed to initialize OpenGL context.", LOG_ERROR);
            return nullptr;
        }
        else
        {
            Log::Message("OpenGL functions succesfully loaded.", LOG_INIT);
            Log::Message("Version - Major: " + std::to_string(GLVersion.major) + " Minor: " + std::to_string(GLVersion.minor), LOG_INIT);
            Log::Message("Driver: " + std::string((char*)glGetString(GL_VENDOR)) + " Renderer: " + std::string((char*)glGetString(GL_RENDERER)), LOG_INIT);
        }

        /* NOTE(Joey):

        Next up is initializing debug output. Debug output allows us to
        give a callback error reporting function to OpenGL that OpenGL
        will call each time an internal error occurs with detailed
        information about the error. This helps us to easily catch
        errors from within OpenGL itself saving us from long hours
        of debugging. For more info see:
        http://learnopengl.com/#!In-Practice/Debugging

        Note that debug output will only be initialized if the
        windowing library initialized an OpenGL context with the
        debug output bit flag set; otherwise this functionality
        is simply ignored. Note that it did not on a 3.3 context,
        it's interesting to figure out why!

        */
        Log::Message("Initializing debug Output.");
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            // NOTE(Joey): we succesfully requested a debug context, now initialize
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr); // NOTE(Joey): defined at bottom of renderer.cpp
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
        }
        else
        {
            Log::Message("Debug output not supported.", LOG_INIT);
        }
        Log::Message("Debug output initialized.");

        // NOTE(Joey): then initialize Cell's core component
        Resources::Init();
        renderer = new Renderer();
        renderer->Init(loadProcFunc);
        
        return renderer;
    }

    void Clean()
    {
        delete renderer;
    }


    // NOTE(Joey): process OpenGL's debug output
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
}