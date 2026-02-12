#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>

// requires `int ret` to be declared before use
#define ERR_JUMP(retcode, jump) \
    do { ret = retcode; goto jump; } while(0)

/**
 * @brief Initialize GLFW, with version hint.
 * @return 0 if success, 1 otherwise.
 */
int init_gl(int version_major, int version_minor);

/**
 * @brief Given a shader whose source is already set via `glShaderSource`,
 * compile that shader, and print out any error that occured.
 * @return 0 if compiled successfully, 1 if not.
 */
int compile_shader(GLuint shader);

// CALLBACKS

/**
 * @brief Register this with `glfwSetErrorCallback`.
 */
void err_callback(int err, const char* desc);
/**
 * @brief Register this with `glfwSetKeyCallback`.
 */
void close_win_callback(GLFWwindow* window, int key, int scancode,
        int action, int mode);

// WINDOW SIZE
/**
 * @brief Set viewport to the width and height of the window.
 */
void set_viewport(GLFWwindow* window);
