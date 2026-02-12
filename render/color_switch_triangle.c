#include "common.h"
#include "render.h"
#include <assert.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

constexpr GLfloat color_list[] = {
    // catppuccin mocha mauve
    198.0f/255.0f, 160.0f/255.0f, 246.0f/255.0f,
    // catppuccin mocha maroon
    237.0f/255.0f, 135.0f/255.0f, 150.0f/255.0f,
    // catppuccin mocha yellow
    238.0f/255.0f, 212.0f/255.0f, 159.0f/255.0f,
    // catppuccin mocha green
    166.0f/255.0f, 218.0f/255.0f, 149.0f/255.0f,
    // catppuccin mocha sky
    145.0f/255.0f, 215.0f/255.0f, 227.0f/255.0f,
    // catppuccin mocha lavender
    183.0f/255.0f, 189.0f/255.0f, 248.0f/255.0f,
};

struct vertices {
    GLfloat points[3 * 3];
    GLfloat colors[3 * 3];
};

static const GLchar vertex_shader_content[] = {
#embed "../shaders/change-color-triangle-vertex.glsl"
, 0
};

static const GLchar fragment_shader_content[] = {
#embed "../shaders/change-color-triangle-fragment.glsl"
, 0
};

static struct vertices vertices = {
    .points = {
        0.0f, 0.25f, 0.0f,
        0.25f, -0.25f, 0.0f,
        -0.25f, -0.25f, 0.0f,
    },
    .colors = { 0 },
};

static void fill_colors(unsigned index) {
    for(unsigned i = 0; i < 3; ++i) {
        for(unsigned j = 0; j < 3; ++j) {
            vertices.colors[3 * i + j] = color_list[3 * index + j];
        }
    }
}

// after the specified amount of frames, change color
static const unsigned change_color_interval = 120;

int render_color_changing_triangle() {
    (void)vertices;
    int ret = 0;
    if(init_gl(4, 6)) {
        ERR_JUMP(1, nuke_glfw);
    }
    glfwSetErrorCallback(err_callback);
    GLFWwindow* window = glfwCreateWindow(600, 480, "Pinging triangle",
            nullptr, nullptr);
    if(!window) {
        fprintf(stderr, "Window creation failed!\n");
        ERR_JUMP(1, nuke_glfw);
    }

    glfwMakeContextCurrent(window);
    int glad_ver = gladLoadGL(glfwGetProcAddress);
    if(glad_ver == 0) {
        fprintf(stderr, "Init OpenGL context failed\n");
        ERR_JUMP(1, nuke_window);
    }
    printf("OpenGL version: %d.%d\n", GLAD_VERSION_MAJOR(glad_ver),
            GLAD_VERSION_MINOR(glad_ver));
    glfwSetKeyCallback(window, close_win_callback);
    glfwSwapInterval(1);
    glClearColor(36.0f/255.0f, 39.0f/255.0f, 58.0f/255.0f, 1.0f);

    // prepping shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const GLchar*[]){vertex_shader_content},
            (const GLint[]){sizeof(vertex_shader_content) - 1});
    if(compile_shader(vertex_shader) != 0) {
        ERR_JUMP(1, nuke_vertex_shader);
    }
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const GLchar*[]){fragment_shader_content},
            (const GLint[]){sizeof(fragment_shader_content) - 1});
    if(compile_shader(fragment_shader) != 0) {
        ERR_JUMP(1, nuke_fragment_shader);
    }

    // vertex array and buffer objects
    GLuint vao = 0;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);
    GLuint points_vbo = 0;
    glCreateBuffers(1, &points_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    // in GLSL (specifically, the vertex shader); layout(locaion = 0) is the
    // point and layout(location = 1) the color
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,
            (GLvoid*)offsetof(struct vertices, points));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0,
            (GLvoid*)offsetof(struct vertices, colors));

    // prepping program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&is_linked);
    if(is_linked == GL_FALSE) {
        fprintf(stderr, "Program fail to link:\n");
        GLint log_size_in = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size_in);
        assert(log_size_in > 0);

        size_t log_size = (size_t)log_size_in;
        GLchar* err_report_buff = (GLchar*)malloc(log_size + 1);
        memset(err_report_buff, 0, log_size + 1);
        glGetProgramInfoLog(program, log_size_in, nullptr, err_report_buff);
        fprintf(stderr, "%s\n", err_report_buff);
        free(err_report_buff);
        ERR_JUMP(1, nuke_program);
    }
    
    unsigned idx = 0;
    unsigned interval = 0;
    while(!glfwWindowShouldClose(window)) {
        set_viewport(window);
        fill_colors(idx);
        // there must be a better way to do this
        glNamedBufferData(points_vbo, sizeof(vertices), (const GLvoid*)&vertices, GL_STATIC_DRAW);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();

        if(interval >= change_color_interval) {
            idx = (idx + 1) % 6;
            interval = 0;
        }
        ++interval;
    }

nuke_program:
    glDeleteProgram(program);
// nuke_vertex_array:
    glDeleteBuffers(1, &points_vbo);
    glDeleteVertexArrays(1, &vao);
nuke_fragment_shader:
    glDeleteShader(fragment_shader);
nuke_vertex_shader:
    glDeleteShader(vertex_shader);
nuke_window:
    glfwDestroyWindow(window);
nuke_glfw:
    glfwTerminate();
    return ret;
}
