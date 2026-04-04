#version 460 core

in vec3 v_world_pos;
in vec3 v_normal;
in vec3 v_color;

out vec4 frag_color;

uniform vec3 u_light_dir;
uniform vec3 u_light_color;
uniform vec3 u_tint; 

void main() {
    vec3 n    = normalize(v_normal);
    float diff = max(dot(n, normalize(u_light_dir)), 0.0);

    vec3 base    = v_color * u_tint;
    vec3 ambient = 0.15 * base;
    vec3 diffuse = diff * u_light_color * base;

    frag_color = vec4(ambient + diffuse, 1.0);
}