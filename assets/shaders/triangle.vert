#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_color;

out vec3 v_color;

layout(std140, binding = 0) uniform Matrices {
    mat4 u_view;
    mat4 u_proj;
};

void main() {
    v_color = a_color;
    gl_Position = vec4(a_position, 1.0);
}