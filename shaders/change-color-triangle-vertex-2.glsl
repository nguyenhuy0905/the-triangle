#version 460 core

layout(location = 0) in vec3 point;
layout(location = 1) uniform vec3 color = vec3(1.0, 1.0, 0.0);

void main() {
    gl_Position = vec4(point, 1.0);
}
