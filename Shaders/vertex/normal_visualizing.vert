#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 5) in mat4 instanceMatrix;

layout (std140) uniform matrices{
    mat4 projection;
    mat4 view;
};

out VS_OUT{
	mat4 projection;
	vec3 normal;
} vs_out;


void main()
{
	mat3 normal_matrix = mat3(transpose(inverse(view * instanceMatrix)));
	vs_out.normal = vec3(vec4(normal_matrix * a_normal, 0.0f));
	vs_out.projection = projection;
	gl_Position = view * instanceMatrix * vec4(a_pos, 1.0);		// vertex position
}