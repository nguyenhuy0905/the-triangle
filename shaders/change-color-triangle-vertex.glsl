#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 color_in;

out vec3 color;

void main() {
    color = color_in;
    gl_Position = vec4(pos, 1.0);
}
