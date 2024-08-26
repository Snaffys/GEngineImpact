#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in mat4 instanceMatrix;

layout (std140) uniform matrices{
    mat4 projection;
    mat4 view;
};

void main()
{
	gl_Position = projection * view * instanceMatrix * vec4(a_pos, 1.0);		// vertex position
}