#version 460 core

out vec4 out_color;
layout(location = 1) uniform vec3 color;

void main() {
    out_color = vec4(color, 1.0);
}
