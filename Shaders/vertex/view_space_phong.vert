#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in vec3 a_normal;

out VS_OUT{
	vec3 vert_color;
	vec2 tex_coord;
	vec3 normal;
	vec3 frag_pos;
	vec3 light_p;
} vs_out;

layout (std140) uniform matrices{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

uniform vec3 light_pos;

void main()
{
	gl_Position = projection * view * model * vec4(a_pos, 1.0);	// vertex position
	vs_out.frag_pos = vec3(view * model * vec4(a_pos, 1.0));	// world space vertex position for fragment shader
	vs_out.normal = mat3(transpose(inverse(view * model))) * a_normal;							// normal for fragment shader
	vs_out.light_p = vec3(view * vec4(light_pos, 1.0));

	vs_out.vert_color = a_color;						// color for fragment shader
	vs_out.tex_coord = a_tex_coord;					// texture for fragment shader		
}