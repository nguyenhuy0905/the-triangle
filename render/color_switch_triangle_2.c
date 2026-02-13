#include "common.h"
#include "render.h"
#include <GLFW/glfw3.h>
#include <assert.h>
#include <glad/gl.h>
#include <stdio.h>

static constexpr GLchar vertex_content[] = {
#embed "../shaders/change-color-triangle-vertex-2.glsl"
    , 0};
static constexpr GLchar frag_content[] = {
#embed "../shaders/change-color-triangle-frag-2.glsl"
    , 0};

static constexpr GLfloat colors[] = {
    // catppuccin mocha mauve
    198.0f/255.0f,
    160.0f/255.0f,
    246.0f/255.0f,
    // catppuccin mocha maroon
    237.0f/255.0f,
    135.0f/255.0f,
    150.0f/255.0f,
    // catppuccin mocha yellow
    238.0f/255.0f,
    212.0f/255.0f,
    159.0f/255.0f,
    // catppuccin mocha green
    166.0f/255.0f,
    218.0f/255.0f,
    149.0f/255.0f,
    // catppuccin mocha sky
    145.0f/255.0f,
    215.0f/255.0f,
    227.0f/255.0f,
    // catppuccin mocha lavender
    183.0f/255.0f,
    189.0f/255.0f,
    248.0f/255.0f,
};

constexpr GLfloat points[] = {
    // top
    0.0f,
    0.25f,
    0.0f,
    // bottom left
    0.25f,
    -0.25f,
    0.0f,
    // bottom right
    -0.25f,
    -0.25f,
    0.0f,
};

static constexpr unsigned change_color_interval = 30;

int render_color_changing_triangle_2() {
    int ret = 0;
    if (init_gl(4, 6) != 0) {
        ERR_JUMP(1, nuke_glfw);
    }

    glfwSetErrorCallback(err_callback);
    GLFWwindow *window =
        glfwCreateWindow(600, 480, "Color switch triangle", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Window creation failed\n");
        ERR_JUMP(1, nuke_glfw);
    }
    glfwMakeContextCurrent(window);

    auto glad_ver = gladLoadGL(glfwGetProcAddress);
    if (glad_ver == 0) {
        fprintf(stderr, "glad init failed!\n");
    }
    printf("OpenGL version %d.%d\n", GLAD_VERSION_MAJOR(glad_ver),
           GLAD_VERSION_MINOR(glad_ver));
    glfwSetKeyCallback(window, close_win_callback);
    glfwSwapInterval(1);
    glClearColor(36.0f / 255.0f, 39.0f / 255.0f, 58.0f / 255.0f, 1.0f);

    // vertex buffer to store coordinates
    GLuint vao = 0;
    glCreateVertexArrays(1, &vao);
    assert(glGetError() == GL_NO_ERROR);
    glBindVertexArray(vao);
    glEnableVertexArrayAttrib(vao, 0);

    GLuint vbo = 0;
    glCreateBuffers(1, &vbo);
    assert(glGetError() == GL_NO_ERROR);
    glNamedBufferData(vbo, sizeof(points), (const GLvoid *)points,
                      GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // shaders
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const GLchar *[]){vertex_content},
                   (const GLint[]){sizeof(vertex_content) - 1});
    if (compile_shader(vertex_shader) != 0) {
        ERR_JUMP(1, nuke_vertex_shader);
    }
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, (const GLchar *[]){frag_content},
                   (const GLint[]){sizeof(frag_content) - 1});
    if (compile_shader(frag_shader) != 0) {
        ERR_JUMP(1, nuke_frag_shader);
    }

    // create program
    GLuint program = glCreateProgram();
    assert(glGetError() == GL_NO_ERROR);
    glAttachShader(program, vertex_shader);
    glAttachShader(program, frag_shader);
    glLinkProgram(program);
    glUseProgram(program);
    glDetachShader(program, vertex_shader);
    glDetachShader(program, frag_shader);

    // get the color uniform
    GLint color_uniform = glGetUniformLocation(program, "color");
    // if you change location of the uniform vec3, change this assert.
    assert(color_uniform == 1);

    unsigned interval = 0;
    unsigned color_offset = 0;
    glUniform3fv(color_uniform, 1, (const GLvoid*){colors});
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        set_viewport(window);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        if(++interval > change_color_interval) {
            color_offset = (color_offset + 1) % (sizeof(colors) / (3 * sizeof(float)));
            glUniform3fv(color_uniform, 1, (const GLvoid*){&colors[3*color_offset]});
            interval = 0;
        }
        glfwSwapBuffers(window);
    }

    glDeleteProgram(program);
nuke_frag_shader:
    glDeleteShader(frag_shader);
nuke_vertex_shader:
    glDeleteShader(vertex_shader);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(window);
nuke_glfw:
    glfwTerminate();
    return ret;
}
