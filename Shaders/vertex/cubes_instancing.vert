#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in mat4 instanceMatrix;

out vec2 tex_coords;
out vec3 normal;
out vec3 frag_pos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * instanceMatrix * vec4(a_pos, 1.0);	// vertex position
	frag_pos = vec3(instanceMatrix * vec4(a_pos, 1.0));						// world space fragment position
	normal = mat3(transpose(inverse(instanceMatrix))) * a_normal;			// normal for fragment shader
	tex_coords = a_tex_coord;												// texture for fragment shader		
}