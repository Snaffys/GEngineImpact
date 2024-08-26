#version 330 core

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in mat4 instanceMatrix;

out VS_OUT{
	vec2 tex_coords;
	vec3 normal;
	vec3 frag_pos;
	vec4 frag_pos_light;
} vs_out;

layout (std140) uniform matrices{
    mat4 projection;
    mat4 view;
};
uniform mat4 light_projection_matrix;

void main()
{
	gl_Position = projection * view * instanceMatrix * vec4(a_pos, 1.0);		// vertex position
	vs_out.frag_pos = vec3(instanceMatrix * vec4(a_pos, 1.0));						// world space fragment position
	vs_out.normal = mat3(transpose(inverse(instanceMatrix))) * a_normal;			// normal for fragment shader
	vs_out.tex_coords = a_tex_coord;										// texture for fragment shader		
	vs_out.frag_pos_light = light_projection_matrix * vec4(vs_out.frag_pos, 1.0f);
}