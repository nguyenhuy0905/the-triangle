#include "common.h"
#include <assert.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct config {
    struct {
        // in milliseconds
        float frame_time;
    } timing;
};

static struct config cfg = {0};

int init_gl(int version_major, int version_minor) {
    if(!glfwInit()) {
        printf("Initialization failed!\n");
        return 1;
    }

    // give me the latest and greatest
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version_major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version_minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    return 0;
}

void set_max_fps(float fps) {
    cfg.timing.frame_time = 1000.0f/fps;
}

float get_max_fps() {
    return 1000.0f/cfg.timing.frame_time;
}

int compile_shader(GLuint shader) {
    glCompileShader(shader);
    // check if we successfully compile the vertex shader
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        fprintf(stderr, "Vertex shader fail to compile:\n");
        GLint log_size_in = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size_in);
        assert(log_size_in > 0);
        
        size_t log_size = (size_t)log_size_in;
        GLchar* err_report_buff = (GLchar*)malloc(log_size + 1);
        memset(err_report_buff, 0, log_size + 1);
        glGetShaderInfoLog(shader, log_size_in, nullptr, err_report_buff);
        fprintf(stderr, "%s\n", err_report_buff);
        free(err_report_buff);
        return 1;
    }
    return 0;
}

void err_callback(int err, const char* desc) {
    fprintf(stderr, "Error: %s\n", desc);
}

void close_win_callback(GLFWwindow* window, int key, int scancode,
        int action, int mode) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void set_viewport(GLFWwindow* window) {
    assert(window != nullptr);
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    assert(width > 0 && height > 0);
    glViewport(0, 0, width, height);
}
