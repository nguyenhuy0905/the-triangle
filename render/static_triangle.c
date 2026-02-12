#include "common.h"
#include "render.h"
#include <assert.h>
// must include glad before GLFW
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const GLchar vertex_shader_content[] = {
#embed "../shaders/triangle-vertex.glsl"
, 0
};

static const GLchar fragment_shader_content[] = {
#embed "../shaders/triangle-fragment.glsl"
, 0
};

static const GLfloat triangle_vertices[] = {
    0.0f, 0.25f, 0.0f,
    0.25f, -0.25f, 0.0f,
    -0.25f, -0.25f, 0.0f
};

int render_static_triangle() {
    int ret = 0;
    if(ret = init_gl(4, 6), ret != 0) {
        ERR_JUMP(1, nuke);
    }
    glfwSetErrorCallback(err_callback);

    GLFWwindow* window = glfwCreateWindow(600, 400, "Hello, World",
            nullptr, nullptr);
    if(!window) {
        fprintf(stderr, "Window or context creation failed!\n");
        ERR_JUMP(1, nuke_glfw);
    }

    glfwMakeContextCurrent(window);
    // NOTE: need a context BEFORE loading glad
    int glad_ver = gladLoadGL(glfwGetProcAddress);
    if(glad_ver == 0) {
        fprintf(stderr, "Init OpenGL context failed\n");
        ERR_JUMP(1, nuke_window);
    }
    printf("OpenGL version: %d.%d\n", GLAD_VERSION_MAJOR(glad_ver),
            GLAD_VERSION_MINOR(glad_ver));

    // preparing shader
    // vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    printf("Vertex shader content: %s\n", vertex_shader_content);
    glShaderSource(vertex_shader, 1, (const GLchar*[]){vertex_shader_content},
            (const GLint[]){sizeof(vertex_shader_content) - 1});
    if(compile_shader(vertex_shader) != 0) {
        ERR_JUMP(1, nuke_vertex_shader);
    }

    // apparently, we need this to hold vertex attributes, so that the
    // rendering pipeline knows what to do with our vertices.
    GLuint vertex_array_obj = 0;
    glCreateVertexArrays(1, &vertex_array_obj);
    glBindVertexArray(vertex_array_obj);
    glEnableVertexArrayAttrib(vertex_array_obj, 0);

    // while this is the thing holding actual vertex data.
    GLuint vertex_buffer_obj = 0;
    glCreateBuffers(1, &vertex_buffer_obj);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_obj);
    // tldr of this line: each vertex attribute applies to 3 floats in the
    // triangle vertices array.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glNamedBufferData(vertex_buffer_obj, sizeof(triangle_vertices),
            (const GLvoid*)triangle_vertices, GL_STATIC_DRAW);

    // fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    printf("Fragment shader content: %s\n", fragment_shader_content);
    glShaderSource(fragment_shader, 1, (const GLchar*[]){fragment_shader_content},
            (const GLint[]){sizeof(fragment_shader_content) - 1});
    if(compile_shader(fragment_shader) != 0) {
        ERR_JUMP(1, nuke_fragment_shader);
    }

    // now create a program to flash(?) the shader
    GLuint program = glCreateProgram();
    // and attach the shader to it
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    // check if there's any program linking error
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
        ERR_JUMP(1, nuke_gl_program);
    }

    // press escape -> escape, mind-blowing
    glfwSetKeyCallback(window, close_win_callback);
    // cap FPS
    glfwSwapInterval(1);
    glClearColor(36.0f/255.0f, 39.0f/255.0f, 58.0f/255.0f, 1.0f);
    while(!glfwWindowShouldClose(window)) {
        set_viewport(window);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

nuke_fragment_shader:
    glDeleteShader(fragment_shader);
//nuke_vertex_buffer:
    glDeleteBuffers(1, &vertex_buffer_obj);
//nuke_vertex_arrays:
    glDeleteVertexArrays(1, &vertex_array_obj);
nuke_gl_program:
    glDeleteProgram(program);
nuke_vertex_shader:
    glDeleteShader(vertex_shader);
nuke_window:
    glfwDestroyWindow(window);
nuke_glfw:
    glfwTerminate();
nuke:
    return ret;
}
