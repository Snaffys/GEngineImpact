#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 5) in mat4 instance_matrix;

layout (std140) uniform matrices{
    mat4 projection;
    mat4 view;
};

uniform float scale;

void main()
{
	gl_Position = projection * view * instance_matrix * vec4(a_pos + a_normal * scale, 1.0f);		// vertex position

}