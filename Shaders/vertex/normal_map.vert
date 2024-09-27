#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_tex_coord;
layout (location = 3) in vec3 a_tangent;
layout (location = 4) in vec3 a_bitangent;
layout (location = 5) in mat4 instanceMatrix;


layout (std140) uniform matrices{
    mat4 projection;
    mat4 view;
};

layout (std140) uniform light_matrix{
    mat4 light_space_matrix_directional;
};

out VS_OUT{
	vec3 frag_pos_tbn;
	vec4 frag_pos_light;
    vec4 frag_pos_light_spot;
    mat3 TBN;
    vec3 frag_pos;
	vec3 normal;
	vec2 tex_coords;
} vs_out;

void main()
{
	vec3 T = normalize(vec3(instanceMatrix * vec4(a_tangent, 0.0f)));
	vec3 N = normalize(vec3(instanceMatrix * vec4(a_normal, 0.0f)));
	T = normalize(T - dot(T,N) * N);
	vec3 B = cross(N, T);
	//B = normalize(vec3(instanceMatrix * vec4(a_bitangent, 0.0f)));
	vs_out.TBN = transpose(mat3(T,B,N));

	gl_Position = projection * view * instanceMatrix * vec4(a_pos, 1.0);

	vs_out.frag_pos_tbn = vs_out.TBN * vec3(instanceMatrix * vec4(a_pos, 1.0));				// tangent space fragment position


	
	vs_out.frag_pos = vec3(instanceMatrix * vec4(a_pos, 1.0));
	vs_out.frag_pos_light = light_space_matrix_directional * vec4(vs_out.frag_pos, 1.0f);
	vs_out.frag_pos_light_spot = projection * view * vec4(vs_out.frag_pos, 1.0f);	

	vs_out.tex_coords = a_tex_coord;	
	vs_out.normal = mat3(transpose(inverse(instanceMatrix))) * a_normal;
}