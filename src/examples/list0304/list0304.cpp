#include <unistd.h>
#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

class application {
private:
    static void APIENTRY debug_callback(GLenum source,
                                        GLenum type,
                                        GLuint id,
                                        GLenum severity,
                                        GLsizei length,
                                        const GLchar* message,
                                        GLvoid* userParam);

public:
    application() {}
    virtual ~application() {}
    // virtual void run(application* the_app)
    void run()
    {
        bool running = true;
        // app = the_app;
        if (!glfwInit())
        {
            fprintf(stderr, "Failed to initialize GLFW\n");
            return;
        }
        init();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, info.majorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, info.minorVersion);

        if (info.flags.robust)
        {
            glfwWindowHint(GLFW_CONTEXT_ROBUSTNESS, GLFW_LOSE_CONTEXT_ON_RESET);
        }
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES, info.samples);
        glfwWindowHint(GLFW_STEREO, info.flags.stereo ? GL_TRUE : GL_FALSE);
        {
            window = glfwCreateWindow(info.windowWidth, info.windowHeight, info.title, info.flags.fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
            if (!window)
            {
                fprintf(stderr, "Failed to open window\n");
                return;
            }
        }

        glfwMakeContextCurrent(window);
        gl3wInit();
        startup();

        do
        {
            render(glfwGetTime());
            glfwSwapBuffers(window);
            glfwPollEvents();
            running &= (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);
            running &= (glfwWindowShouldClose(window) != GL_TRUE);
        } while (running);

        shutdown();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    virtual void init()
    {
        strcpy(info.title, "OpenGL SuperBible Example");
        info.windowWidth = 800;
        info.windowHeight = 600;
        info.majorVersion = 4;
        info.minorVersion = 5;
        info.samples = 0;
        info.flags.all = 0;
        info.flags.cursor = 1;
    }

    virtual void startup() { }

    virtual void render(double currentTime) { }

    virtual void shutdown() { }

    void setWindowTitle(const char * title) { glfwSetWindowTitle(window, title); }

    virtual void onResize(int w, int h)
    {
        info.windowWidth = w;
        info.windowHeight = h;
    }





    void getMousePosition(int& x, int& y)
    {
        double dx, dy;
        glfwGetCursorPos(window, &dx, &dy);
        x = static_cast<int>(floor(dx));
        y = static_cast<int>(floor(dy));
    }

public:
    struct APPINFO
    {
        char title[128];
        int windowWidth;
        int windowHeight;
        int majorVersion;
        int minorVersion;
        int samples;
        union
        {
            struct
            {
                unsigned int    fullscreen  : 1;
                unsigned int    vsync       : 1;
                unsigned int    cursor      : 1;
                unsigned int    stereo      : 1;
                unsigned int    debug       : 1;
                unsigned int    robust      : 1;
            };
            unsigned int        all;
        } flags;
    };

protected:
    APPINFO     info;
    static      application * app;
    GLFWwindow* window;
};

class tessellatedtri_app : public application
{
    void init()
    {
        static const char title[] = "OpenGL SuperBible - Tessellated Triangle";
        application::init();
        memcpy(info.title, title, sizeof(title));
    }

    virtual void startup()
    {
        static const char * vs_source[] =
        {
            "#version 450 core                                                 \n"
            "layout (location = 0) in vec4 offset;  \n"
            "layout (location = 1) in vec4 color;   \n"
            "out vec4 vs_color; \n"
            "void main(void)                                                   \n"
            "{                                                                 \n"
            "    const vec4 vertices[] = vec4[](vec4( 0.25, -0.25, 0.5, 1.0),  \n"
            "                                   vec4(-0.25, -0.25, 0.5, 1.0),  \n"
            "                                   vec4( 0.25,  0.25, 0.5, 1.0)); \n"
            "                                                                  \n"
            "    gl_Position = vertices[gl_VertexID] + offset;                          \n"
            "    vs_color = color; \n"
            "}                                                                 \n"
        };

        static const char * tcs_source[] =
        {
            "#version 450 core                                                                 \n"
            "layout (vertices = 3) out;                                                        \n"
            "       in vec4 vs_colors [];\n"
            "      out vec4 tcs_colors [3];\n"
            "patch out vec4 patch_color;   \n"
            "void main(void)                                                                   \n"
            "{                                                                                 \n"
            // gl_TessLevelInner and gl_TessLevelOuter should only be set once
            "    if (gl_InvocationID == 0)                                                     \n"
            "    {                                                                             \n"
            "        gl_TessLevelInner[0] = 3.0;                                               \n"
            "        gl_TessLevelOuter[0] = 2.0;                                               \n"
            "        gl_TessLevelOuter[1] = 2.0;                                               \n"
            "        gl_TessLevelOuter[2] = 2.0;                                               \n"
            "    }                                                                             \n"
            // "    patch_color = vec4(.2, .3, .8); \n"
            // "    patch_color = vs_colors [gl_InvocationID]; \n"
            // "    tcs_colors[gl_InvocationID] = vs_colors[gl_InvocationID]; \n"
            "    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;     \n"
            "}                                                                                 \n"
        };

        static const char * tes_source[] =
        {
            "#version 450 core                                                                 \n"
            "layout (triangles, equal_spacing, cw) in;                                         \n"
            // "patch in vec4 patch_color;  \n"
            // "     out vec4 tes_color;  \n"
            "void main(void)                                                                   \n"
            "{                                                                                 \n"
            "    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +                       \n"
            "                  (gl_TessCoord.y * gl_in[1].gl_Position) +                       \n"
            "                  (gl_TessCoord.z * gl_in[2].gl_Position);                        \n"
            // "    tes_color = patch_color; \n"
            "}                                                                                 \n"
        };

        static const char * fs_source[] =
        {
            "#version 450 core                                                 \n"
            "in vec4 tes_color; \n"
            "out vec4 color;                                                   \n"
            "void main(void)                                                   \n"
            "{                                                                 \n"
            "    color = vec4(0.2, 0.2, .4, 1.0);                             \n"
            // "    color = tes_color;"
            "}                                                                 \n"
        };

        program = glCreateProgram();
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, vs_source, NULL);
        glCompileShader(vs);

        GLuint tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tcs, 1, tcs_source, NULL);
        glCompileShader(tcs);

        GLuint tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tes, 1, tes_source, NULL);
        glCompileShader(tes);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, fs_source, NULL);
        glCompileShader(fs);

        glAttachShader(program, vs);
        glAttachShader(program, tcs);
        glAttachShader(program, tes);
        glAttachShader(program, fs);

        glLinkProgram(program);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    virtual void render(double currentTime)
    {
        float color_var = ((float)sin(currentTime) + 1.f) / 2.f;
        bg_color[0] = color_var;
        glClearBufferfv(GL_COLOR, 0, bg_color);
        glUseProgram(program);

        float offset[] = {
            (float) sin(currentTime) * .5f,
            (float) cos(currentTime) * .5f,
            (float) cos(currentTime) * .5f,
            0.0f
        };
        glVertexAttrib4fv(0, offset);

        fg_color[2] = color_var;
        glVertexAttrib4fv(1, fg_color);

        glDrawArrays(GL_PATCHES, 0, 3);
    }

    virtual void shutdown()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteProgram(program);
    }

private:
    float bg_color[3] = {1.f, .7f, .8f};
    float fg_color[3] = {.2f, .7f, .2f};
    GLuint          program;
    GLuint          vao;
};


int main(int argc, const char ** argv)              
{                                                  
    tessellatedtri_app *app = new tessellatedtri_app();             
    app->run();
    delete app;                                    
    return 0;                                      
}
