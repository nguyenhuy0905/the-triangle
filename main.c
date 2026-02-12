// must include glad first
#include <assert.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void err_callback(int err, const char* desc) {
    fprintf(stderr, "Error: %s\n", desc);
}

static void win_key_callback(GLFWwindow* window, int key, int scancode,
        int action, int mode) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

static const GLchar vertex_shader_content[] = {
#embed "triangle-vertex.glsl"
, 0
};

static const GLchar fragment_shader_content[] = {
#embed "triangle-fragment.glsl"
, 0
};

static const GLfloat triangle_vertices[] = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f
};

int main() {
    if(!glfwInit()) {
        printf("Initialization failed!\n");
        return 69;
    }

    glfwSetErrorCallback(err_callback);
    // give me the latest and greatest
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(600, 400, "Hello, World",
            nullptr, nullptr);
    if(!window) {
        fprintf(stderr, "Window or context creation failed!\n");
        goto nuke_glfw;
    }

    glfwMakeContextCurrent(window);
    // NOTE: need a context BEFORE loading glad
    int glad_ver = gladLoadGL(glfwGetProcAddress);
    if(glad_ver == 0) {
        fprintf(stderr, "Init OpenGL context failed\n");
        goto nuke_window;
    }
    printf("OpenGL version: %d.%d\n", GLAD_VERSION_MAJOR(glad_ver),
            GLAD_VERSION_MINOR(glad_ver));

    // preparing shader
    // vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    printf("Vertex shader content: %s\n", vertex_shader_content);
    glShaderSource(vertex_shader, 1, (const GLchar*[]){vertex_shader_content},
            (const GLint[]){sizeof(vertex_shader_content) - 1});
    glCompileShader(vertex_shader);
    // check if we successfully compile the vertex shader
    GLint success = 0;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        fprintf(stderr, "Vertex shader fail to compile:\n");
        GLint log_size = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_size);
        GLchar* err_report_buff = (GLchar*)malloc(log_size + 1);
        memset(err_report_buff, 0, log_size + 1);
        glGetShaderInfoLog(vertex_shader, log_size, nullptr, err_report_buff);
        fprintf(stderr, "%s\n", err_report_buff);
        free(err_report_buff);
        goto nuke_vertex_shader;
    }
    // compilation success!!

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
    glCompileShader(fragment_shader);
    // check if we successfully compile the fragment shader
    success = 0;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        fprintf(stderr, "fragment shader fail to compile:\n");
        GLint log_size = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_size);
        GLchar* err_report_buff = (GLchar*)malloc(log_size + 1);
        memset(err_report_buff, 0, log_size + 1);
        glGetShaderInfoLog(fragment_shader, log_size, nullptr, err_report_buff);
        fprintf(stderr, "%s\n", err_report_buff);
        free(err_report_buff);
        goto nuke_fragment_shader;
    }
    // compilation success!!

    // now create a program to flash(?) the shader
    GLuint program = glCreateProgram();
    // and attach the shader to it
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&is_linked);
    if(is_linked == GL_FALSE) {
        fprintf(stderr, "Program fail to link:\n");
        GLint log_size = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
        GLchar* err_report_buff = (GLchar*)malloc(log_size + 1);
        memset(err_report_buff, 0, log_size + 1);
        glGetProgramInfoLog(program, log_size, nullptr, err_report_buff);
        fprintf(stderr, "%s\n", err_report_buff);
        free(err_report_buff);
        goto nuke_gl_program;
    }

    // press escape -> escape, mind-blowing
    glfwSetKeyCallback(window, win_key_callback);
    // cap FPS
    glfwSwapInterval(1);
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

nuke_fragment_shader:
    glDetachShader(program, fragment_shader);
    glDeleteShader(fragment_shader);
//nuke_vertex_buffer:
    glDeleteBuffers(1, &vertex_buffer_obj);
//nuke_vertex_arrays:
    glDeleteVertexArrays(1, &vertex_array_obj);
nuke_gl_program:
    glDetachShader(program, vertex_shader);
    glDeleteProgram(program);
nuke_vertex_shader:
    glDeleteShader(vertex_shader);
nuke_window:
    glfwDestroyWindow(window);
nuke_glfw:
    glfwTerminate();
    return 0;
}
