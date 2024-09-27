#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 5) in mat4 instance_matrix;



void main()
{
    gl_Position = instance_matrix * vec4(a_pos, 1.0);
} 