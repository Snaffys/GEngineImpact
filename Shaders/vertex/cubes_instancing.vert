#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in mat4 instanceMatrix;


layout (std140) uniform matrices{
    mat4 projection;
    mat4 view;
};

layout (std140) uniform light_matrix{
    mat4 light_space_matrix_directional;
};

out VS_OUT{
	vec2 tex_coords;
	vec3 normal;
	vec3 frag_pos;
	vec4 frag_pos_light;
	vec4 frag_pos_light_spot;
} vs_out;

out VS_OUT_G{
	vec2 tex_coords;
} vs_out_g;

void main()
{
	gl_Position = projection * view * instanceMatrix * vec4(a_pos, 1.0);	// vertex position
	vs_out.frag_pos = vec3(instanceMatrix * vec4(a_pos, 1.0));						// world space fragment position
	vs_out.normal = mat3(transpose(inverse(instanceMatrix))) * a_normal;			// normal for fragment shader	
	vs_out.tex_coords = a_tex_coord;												// texture for fragment shader		
	vs_out.frag_pos_light = light_space_matrix_directional * vec4(vs_out.frag_pos, 1.0f);
	vs_out.frag_pos_light_spot = projection * view * vec4(vs_out.frag_pos, 1.0f);
}