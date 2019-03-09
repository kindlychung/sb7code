#include "sb7.h"

GLuint compile_shaders(void) {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
    static const GLchar* vertex_shader_source[] = {
        "#version 450 core \n"
        "void main(void) {\n"
        "    const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, .5, 1.0), \n"
        "                                     vec4(-0.25, -0.25, .5, 1.0), \n"
        "                                     vec4(0., 0.35, .5, 1.0)); \n"
        "    gl_Position = vertices[gl_VertexID]; \n"
        "}\n"
    };
    static const GLchar* fragment_shader_source[] = {
        "#version 450 core \n"
        "out vec4 color; \n"
        "void main(void) {\n"
        "    color = vec4(.0, .8, 1., 1.);\n"
        "}\n"
    };
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
    glCompileShader(vertex_shader);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
    glCompileShader(fragment_shader);
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

class my_application : public sb7::application {
   private:
    float color[3] = {1.f, .7f, .5f};
    GLuint rendering_program;
    GLuint vertex_array_object;

   public:
    my_application();
    void render(double currentTime);
    void startup() {
        rendering_program = compile_shaders();
        glCreateVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);
    }
    void shutdown() {
        glDeleteVertexArrays(1, &vertex_array_object);
        glDeleteProgram(rendering_program);
        glDeleteVertexArrays(1, &vertex_array_object);
    }
};

my_application::my_application() {}

void my_application::render(double currentTime) {
    float red = ((float)sin(currentTime) + 1.f) / 2.f;
    color[0] = red;
    glClearBufferfv(GL_COLOR, 0, color);
    glUseProgram(rendering_program);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

DECLARE_MAIN(my_application);