#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in vec3 a_normal;

out vec3 vert_color;
out vec2 tex_coord;

out vec3 normal;
out vec3 frag_pos;
out vec3 light_p;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 light_pos;

void main()
{
	gl_Position = projection * view * model * vec4(a_pos, 1.0);	// vertex position
	frag_pos = vec3(view * model * vec4(a_pos, 1.0));	// world space vertex position for fragment shader
	normal = mat3(transpose(inverse(view * model))) * a_normal;							// normal for fragment shader
	light_p = vec3(view * vec4(light_pos, 1.0));

	vert_color = a_color;						// color for fragment shader
	tex_coord = a_tex_coord;					// texture for fragment shader		
}