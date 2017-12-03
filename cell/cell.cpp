#include "cell.h"

#include <utility/logging/log.h>

#include <glfw/glfw3.h>
#include "imgui/imgui.h"

#include "renderer/PostProcessor.h"

namespace Cell
{
    void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam);
    bool ImGui_ImplGlfwGL3_Init(GLFWwindow* window, bool install_callbacks);
    void ImGui_ImplGlfwGL3_RenderDrawLists(ImDrawData* draw_data);
    void ImGui_ImplGlfwGL3_Shutdown();
    void ImGui_ImplGlfwGL3_NewFrame();
    bool ImGui_ImplGlfwGL3_CreateFontsTexture();
    bool ImGui_ImplGlfwGL3_CreateDeviceObjects();
    void ImGui_ImplGlfwGL3_Shutdown();
    void ImGui_ImplGlfwGL3_CharCallback(GLFWwindow*, unsigned int c);

    // IMGUI data
    static GLFWwindow*  g_Window = NULL;
    static double       g_Time = 0.0f;
    static bool         g_MousePressed[3] = { false, false, false };
    static float        g_MouseWheel = 0.0f;
    static GLuint       g_FontTexture = 0;
    static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
    static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
    static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
    static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;

    // hold a pointer to the renderer so we can delete it during cleanup
    Renderer* renderer;
    Renderer* Init(GLFWwindow* window, GLADloadproc loadProcFunc)
    {
        /* 

          Before doing anything with OpenGL we have to load all its functions into stored function 
          pointers from the OpenGL driver. Windows by default only hosts a small set of OpenGL 
          functions in its library so all the more 'modern' functions have to be manually loaded/
          requested from the graphics cards' OpenGL driver itself.
          
          This is a cumbersome work as we generally have to store all the required function 
          prototypes, function pointer storage and all the enum / unique ids of the OpenGL 
          functionality and on top of that we have to query for the function's memory location for
          each and every one of these functions.
          
          For this reason we use a library called GLAD http://glad.dav1d.de/ <- their webservice 
          for generating header file.

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

        /* 

          Next up is initializing debug output. Debug output allows us to give a callback error 
          reporting function to OpenGL that OpenGL will call each time an internal error occurs 
          with detailed information about the error. This helps us to easily catch errors from 
          within OpenGL itself saving us from long hours of debugging. For more info see:
          http://learnopengl.com/#!In-Practice/Debugging
          
          Note that debug output will only be initialized if the windowing library initialized an
          OpenGL context with the debug output bit flag set; otherwise this functionality is simply
          ignored. Note that it did not on a 3.3 context, it's interesting to figure out why!

        */
        Log::Message("Initializing debug Output.", LOG_INIT);
        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            // we succesfully requested a debug context, now initialize
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback((GLDEBUGPROC)glDebugOutput, nullptr); 
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
        }
        else
        {
            Log::Message("Debug output not supported.", LOG_INIT);
        }
        Log::Message("Debug output initialized.", LOG_INIT);

        // then initialize Cell's core component
        Resources::Init();
        renderer = new Renderer();
        renderer->Init(loadProcFunc);

        // initialize imgui
        ImGui_ImplGlfwGL3_Init(window, true);
        
        return renderer;
    }

    void Clean()
    {
        ImGui_ImplGlfwGL3_Shutdown();

        delete renderer;
        // clean up all still stored scene nodes managed globally by static scene class
        Scene::Clear();
        Resources::Clean();
    }

    void NewFrame()
    {
        ImGui_ImplGlfwGL3_NewFrame();
    }

    bool show_test_window = true;
    bool show_another_window = false;

    void RenderGUI()
    {
        ImGui::Begin("Renderer", (bool*)1);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        if (ImGui::CollapsingHeader("General Options"))
        {
            ImGui::Checkbox("IrradianceGI", &renderer->IrradianceGI);
            ImGui::Checkbox("Shadows", &renderer->Shadows);
            ImGui::Checkbox("Lights", &renderer->Lights);
            ImGui::Checkbox("Render Light Shapes", &renderer->RenderLights);
        }
        if (ImGui::CollapsingHeader("Post-processing"))
        {
            ImGui::Checkbox("SSAO", &renderer->GetPostProcessor()->SSAO);
            ImGui::Checkbox("Bloom", &renderer->GetPostProcessor()->Bloom);
            ImGui::Checkbox("Motion Blur", &renderer->GetPostProcessor()->MotionBlur);
            //ImGui::Checkbox("SSR", &renderer->GetPostProcessor()->SSR);
            //ImGui::Checkbox("TXAA", &renderer->GetPostProcessor()->TXAA);
            ImGui::Checkbox("Vignette", &renderer->GetPostProcessor()->Vignette);
            ImGui::Checkbox("Sepia", &renderer->GetPostProcessor()->Sepia);
        }
        if (ImGui::CollapsingHeader("Debug visualization"))
        {
            ImGui::Checkbox("Wireframe", &renderer->Wireframe);
            ImGui::Checkbox("Light Volumes", &renderer->LightVolumes);
            ImGui::Checkbox("Ambient Probes", &renderer->RenderProbes);
        }
        ImGui::End();

        ImGui::Render();
    }

    void InputMouse(int button, int action)
    {
        if (action == GLFW_PRESS && button >= 0 && button < 3)
            g_MousePressed[button] = true;
    }

    void InputKey(int key, int action)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
        if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;

        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
    }

    void InputScroll(float scrollOffset)
    {
        g_MouseWheel += scrollOffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
    }

    // process OpenGL's debug output
    void APIENTRY glDebugOutput(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar *message,
        void *userParam)
    {
        // ignore non-significant error/warning codes
        if (
            id == 131169 || id == 131185 || id == 131218 || id == 131204 || id || // driver-specific non-significant error codes
            id == 2000 || id == 2001 || id == 2265 // shader compilation error codes; ignore as already managed from shader object
        )
        {
            return;
        }

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

        // only log a message a maximum of 3 times (as it'll keep spamming the message queue with
        // the same error message)
        static unsigned int msgCount = 0;
        if(msgCount++ < 3)
        {
            Log::Message(logMessage, type == GL_DEBUG_TYPE_ERROR ? LOG_ERROR : LOG_WARNING);
        }
    }

    // This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
    // Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL engine that doesn't do so. 
    // If text or lines are blurry when integrating ImGui in your engine: in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
    void ImGui_ImplGlfwGL3_RenderDrawLists(ImDrawData* draw_data)
    {
        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        ImGuiIO& io = ImGui::GetIO();
        int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
        int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
        if (fb_width == 0 || fb_height == 0)
            return;
        draw_data->ScaleClipRects(io.DisplayFramebufferScale);

        // Backup GL state
        GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
        glActiveTexture(GL_TEXTURE0);
        GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
        GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
        GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
        GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
        GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
        GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
        GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
        GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
        GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
        GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
        GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Setup viewport, orthographic projection matrix
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        const float ortho_projection[4][4] =
        {
            { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
            { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
            { 0.0f,                  0.0f,                  -1.0f, 0.0f },
            { -1.0f,                  1.0f,                   0.0f, 1.0f },
        };
        glUseProgram(g_ShaderHandle);
        glUniform1i(g_AttribLocationTex, 0);
        glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
        glBindVertexArray(g_VaoHandle);
        glBindSampler(0, 0); // Rely on combined texture/sampler state.

        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            const ImDrawIdx* idx_buffer_offset = 0;

            glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback)
                {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                    glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
                }
                idx_buffer_offset += pcmd->ElemCount;
            }
        }

        // Restore modified GL state
        glUseProgram(last_program);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindSampler(0, last_sampler);
        glActiveTexture(last_active_texture);
        glBindVertexArray(last_vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
        if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, last_polygon_mode[0]);
        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
        glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    }

    void ImGui_ImplGlfwGL3_CharCallback(GLFWwindow*, unsigned int c)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (c > 0 && c < 0x10000)
            io.AddInputCharacter((unsigned short)c);
    }

    bool ImGui_ImplGlfwGL3_CreateFontsTexture()
    {
        // Build texture atlas
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

                                                                  // Upload texture to graphics system
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &g_FontTexture);
        glBindTexture(GL_TEXTURE_2D, g_FontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        // Store our identifier
        io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

        // Restore state
        glBindTexture(GL_TEXTURE_2D, last_texture);

        return true;
    }

    bool ImGui_ImplGlfwGL3_CreateDeviceObjects()
    {
        // Backup GL state
        GLint last_texture, last_array_buffer, last_vertex_array;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

        const GLchar *vertex_shader =
            "#version 330\n"
            "uniform mat4 ProjMtx;\n"
            "in vec2 Position;\n"
            "in vec2 UV;\n"
            "in vec4 Color;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "	Frag_UV = UV;\n"
            "	Frag_Color = Color;\n"
            "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar* fragment_shader =
            "#version 330\n"
            "uniform sampler2D Texture;\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
            "}\n";

        g_ShaderHandle = glCreateProgram();
        g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
        g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(g_VertHandle, 1, &vertex_shader, 0);
        glShaderSource(g_FragHandle, 1, &fragment_shader, 0);
        glCompileShader(g_VertHandle);
        glCompileShader(g_FragHandle);
        glAttachShader(g_ShaderHandle, g_VertHandle);
        glAttachShader(g_ShaderHandle, g_FragHandle);
        glLinkProgram(g_ShaderHandle);

        g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
        g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
        g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
        g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
        g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

        glGenBuffers(1, &g_VboHandle);
        glGenBuffers(1, &g_ElementsHandle);

        glGenVertexArrays(1, &g_VaoHandle);
        glBindVertexArray(g_VaoHandle);
        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        glEnableVertexAttribArray(g_AttribLocationPosition);
        glEnableVertexAttribArray(g_AttribLocationUV);
        glEnableVertexAttribArray(g_AttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
        glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

        ImGui_ImplGlfwGL3_CreateFontsTexture();

        // Restore modified GL state
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindVertexArray(last_vertex_array);

        return true;
    }

    void ImGui_ImplGlfwGL3_InvalidateDeviceObjects()
    {
        if (g_VaoHandle) glDeleteVertexArrays(1, &g_VaoHandle);
        if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
        if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
        g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

        if (g_ShaderHandle && g_VertHandle) glDetachShader(g_ShaderHandle, g_VertHandle);
        if (g_VertHandle) glDeleteShader(g_VertHandle);
        g_VertHandle = 0;

        if (g_ShaderHandle && g_FragHandle) glDetachShader(g_ShaderHandle, g_FragHandle);
        if (g_FragHandle) glDeleteShader(g_FragHandle);
        g_FragHandle = 0;

        if (g_ShaderHandle) glDeleteProgram(g_ShaderHandle);
        g_ShaderHandle = 0;

        if (g_FontTexture)
        {
            glDeleteTextures(1, &g_FontTexture);
            ImGui::GetIO().Fonts->TexID = 0;
            g_FontTexture = 0;
        }
    }

    bool ImGui_ImplGlfwGL3_Init(GLFWwindow* window, bool install_callbacks)
    {
        g_Window = window;

        ImGuiIO& io = ImGui::GetIO();
        io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
        io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
        io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
        io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
        io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
        io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
        io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
        io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
        io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
        io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
        io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
        io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
        io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
        io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
        io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
        io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
        io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
        io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
        io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

        io.RenderDrawListsFn = ImGui_ImplGlfwGL3_RenderDrawLists;       // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.

        if (install_callbacks)
        {
            glfwSetCharCallback(window, ImGui_ImplGlfwGL3_CharCallback);
        }

        return true;
    }

    void ImGui_ImplGlfwGL3_Shutdown()
    {
        ImGui_ImplGlfwGL3_InvalidateDeviceObjects();
        ImGui::Shutdown();
    }

    void ImGui_ImplGlfwGL3_NewFrame()
    {
        if (!g_FontTexture)
            ImGui_ImplGlfwGL3_CreateDeviceObjects();

        ImGuiIO& io = ImGui::GetIO();

        // Setup display size (every frame to accommodate for window resizing)
        int w, h;
        int display_w, display_h;
        glfwGetWindowSize(g_Window, &w, &h);
        glfwGetFramebufferSize(g_Window, &display_w, &display_h);
        io.DisplaySize = ImVec2((float)w, (float)h);
        io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

        // Setup time step
        double current_time = glfwGetTime();
        io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
        g_Time = current_time;

        // Setup inputs
        // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
        if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED))
        {
            if (io.WantMoveMouse)
            {
                glfwSetCursorPos(g_Window, (double)io.MousePos.x, (double)io.MousePos.y);   // Set mouse position if requested by io.WantMoveMouse flag (used when io.NavMovesTrue is enabled by user and using directional navigation)
            }
            else
            {
                double mouse_x, mouse_y;
                glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
                io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);   // Get mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
            }
        }
        else
        {
            io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
        }

        for (int i = 0; i < 3; i++)
        {
            io.MouseDown[i] = g_MousePressed[i] || glfwGetMouseButton(g_Window, i) != 0;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
            g_MousePressed[i] = false;
        }

        io.MouseWheel = g_MouseWheel;
        g_MouseWheel = 0.0f;

        // Hide OS mouse cursor if ImGui is drawing it
        //glfwSetInputMode(g_Window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

        // Start the frame
        ImGui::NewFrame();
    }
}