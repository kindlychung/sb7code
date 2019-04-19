#include "sb7.h"

GLuint compile_shaders(void) {
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint program;
    static const GLchar* vertex_shader_source[] = {
        "#version 450 core \n"
        // 'offset' is an vector input vertex attribute
        // it's put in location 0
        "layout (location = 0) in vec4 offset;  \n"
        "layout (location = 1) in vec4 color;   \n"
        "out  vec4 vs_color;"
        "void main(void) {\n"
        "    const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, .5, 1.0), \n"
        "                                     vec4(-0.25, -0.25, .5, 1.0), \n"
        "                                     vec4(0., 0.35, .5, 1.0)); \n"
        "    gl_Position = vertices[gl_VertexID] + offset; \n"
        "    vs_color = color; \n"
        "}\n"};

    static const char* tcs_source[] = {
        "#version 450 core \n"
        "layout (vertices = 3) out;   \n"
        "in  vec4 vs_color[];\n"
        "out vec4 tcs_colors[]; \n"
        "void main(void) {      \n"
        // gl_TessLevelInner and gl_TessLevelOuter should only be set once
        "    if (gl_InvocationID == 0) \n"
        "    {    \n"
        "        gl_TessLevelInner[0] = 3.0;  \n"
        "        gl_TessLevelOuter[0] = 2.0; \n"
        "        gl_TessLevelOuter[1] = 2.0; \n"
        "        gl_TessLevelOuter[2] = 2.0; \n"
        "    } \n"
        "    tcs_colors[gl_InvocationID] = vs_color[gl_InvocationID]; \n"
        "    gl_out[gl_InvocationID].gl_Position = "
        "           gl_in[gl_InvocationID].gl_Position; \n"
        "}\n                   "};

    static const char* tes_source[] = {
        "#version 450 core    \n"
        "layout (triangles, equal_spacing, cw) in;   \n"
        "in  vec4 tcs_colors[]; \n"
        "out vec4 tes_colors; \n"
        "void main(void)        \n"
        "{                \n"
        "    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) +   \n"
        "                  (gl_TessCoord.y * gl_in[1].gl_Position) +  \n"
        "                  (gl_TessCoord.z * gl_in[2].gl_Position);   \n"
        "    tes_colors = gl_TessCoord.x * tcs_colors[0] + \n"
        "      gl_TessCoord.y * tcs_colors[1] + \n"
        "      gl_TessCoord.z * tcs_colors[2]; \n"
        "}     "};

    static const GLchar* fragment_shader_source[] = {
        "#version 450 core \n"
        "out vec4 color; \n"
        "in  vec4 tes_colors; \n"
        "void main(void) {\n"
        "    color = tes_colors;\n"
        "}\n"};
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    GLuint tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
    glShaderSource(tcs, 1, tcs_source, NULL);
    glCompileShader(tcs);

    GLuint tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
    glShaderSource(tes, 1, tes_source, NULL);
    glCompileShader(tes);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, tcs);
    glAttachShader(program, tes);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(tcs);
    glDeleteShader(tes);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

class my_application : public sb7::application {
   private:
    float bg_color[3] = {1.f, .7f, .8f};
    float fg_color[3] = {.2f, .1f, .2f};
    GLuint rendering_program;
    GLuint vertex_array_object;

   public:
    my_application();
    void render(double currentTime);
    void startup() {
        rendering_program = compile_shaders();
        glCreateVertexArrays(1, &vertex_array_object);
        glBindVertexArray(vertex_array_object);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    void shutdown() {
        glDeleteVertexArrays(1, &vertex_array_object);
        glDeleteProgram(rendering_program);
    }
};

my_application::my_application() {}

void my_application::render(double currentTime) {
    float color_var = ((float)sin(currentTime) + 1.f) / 2.f;
    bg_color[0] = color_var;
    glClearBufferfv(GL_COLOR, 0, bg_color);
    fg_color[2] = color_var;
    glUseProgram(rendering_program);
    float attrib[] = {(float)sin(currentTime) * .5f,
                      (float)cos(currentTime) * .5f, .0f, .0f};
    glVertexAttrib4fv(0, attrib);
    glVertexAttrib4fv(1, fg_color);
    glDrawArrays(GL_PATCHES, 0, 3);
}

DECLARE_MAIN(my_application);
