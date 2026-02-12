// must include glad first
#include "render/common.h"
#include "render/render.h"
#include <assert.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    return render_static_triangle();
}
