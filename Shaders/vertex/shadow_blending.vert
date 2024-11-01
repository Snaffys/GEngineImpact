#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in mat4 instanceMatrix;


out VS_OUT{
    vec2 tex_coords;
} vs_out;

layout (std140) uniform light_matrix{
    mat4 light_space_matrix_directional;
};

void main()
{
    gl_Position = light_space_matrix_directional * instanceMatrix * vec4(a_pos, 1.0);
    vs_out.tex_coords = a_tex_coord;
} 