#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in mat4 instance_matrix;


layout (std140) uniform matrices{
    mat4 projection;
    mat4 view;
};

out VS_OUT{
	vec3 frag_pos;
	vec2 tex_coords;
	vec3 normal;
} vs_out;


void main()
{
//	vec4 world_pos = instance_matrix * vec4(a_pos, 1.0f);
//	vs_out.frag_pos = world_pos.xyz;
//	vs_out.tex_coords = a_tex_coord;
//	
//	mat3 normal_matrix = transpose(inverse(mat3(instance_matrix)));
//	vs_out.normal = normal_matrix * a_normal;
//	
//	gl_Position = projection * view * world_pos;


	vec4 view_pos = view * instance_matrix * vec4(a_pos, 1.0f);
	vs_out.frag_pos = view_pos.xyz;
	vs_out.tex_coords = a_tex_coord;

	mat3 normal_matrix = transpose(inverse(mat3(view * instance_matrix)));
	vs_out.normal = normal_matrix * a_normal;

	gl_Position = projection * view_pos;
}