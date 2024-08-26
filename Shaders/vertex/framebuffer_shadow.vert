#version 330 core

layout (location = 0) in vec3 a_pos;

layout (std140) uniform light_matrix{
    mat4 light_space_matrix;
};

uniform mat4 model;

void main()
{
    gl_Position = light_space_matrix * model * vec4(a_pos, 1.0);
} 