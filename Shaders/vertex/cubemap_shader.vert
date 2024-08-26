#version 330 core
layout (location = 0) in vec3 a_pos;

out VS_OUT{
    vec3 tex_coords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;


void main(){
    vs_out.tex_coords = a_pos;
    vec4 pos = projection * view * vec4(a_pos, 1.0);
    gl_Position = pos.xyww;
}