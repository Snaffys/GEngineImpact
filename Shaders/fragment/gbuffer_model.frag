////#version 330 core
////layout (location = 0) out vec3 g_position;
////layout (location = 1) out vec3 g_normal;
////layout (location = 2) out vec4 g_albedo_spec;
////
////in VS_OUT{
////	vec2 tex_coords;
////	vec3 normal;
////	vec3 frag_pos;
////} vs_in;
////
////struct Material{
////	sampler2D texture_diffuse1;
////	sampler2D texture_specular1;
////};
////
////uniform Material material;
////
////void main()
////{    
////    // store the fragment position vector in the first gbuffer texture
////    g_position = vs_in.frag_pos;
////
////
////    // also store the per-fragment normals into the gbuffer
////    g_normal = normalize(vs_in.normal);
////
////
////    // and the diffuse per-fragment color
////    g_albedo_spec.rgb = texture(material.texture_diffuse1, vs_in.tex_coords).rgb;
////    // store specular intensity in gAlbedoSpec's alpha component
////    g_albedo_spec.a = texture(material.texture_specular1, vs_in.tex_coords).r;
////}
//
//
//
//
//#version 330 core
//
//// output
//layout (location = 0) out vec3 g_position;
//layout (location = 1) out vec3 g_normal;
//layout (location = 2) out vec4 g_albedo_spec;
//
//// material structure
//struct Material{
//	sampler2D texture_diffuse1;
//	sampler2D texture_specular1;
//	float shininess;
//    sampler2D normal_map1;
//    sampler2D displacement_map1;
//};
//
//
//in VS_OUT{
//	vec3 frag_pos_tbn;
//	vec4 frag_pos_light;
//    vec4 frag_pos_light_spot;
//    mat3 TBN;
//    vec3 frag_pos;
//	vec3 normal;
//	vec2 tex_coords;
//} fs_in;
//
//#define NR_POINT_LIGHTS 4
//
//
//// structures objects
//uniform Material material;
//
//
//uniform bool normal_map;
//uniform bool parallax_map;
//
//
//uniform float height_scale;
//
//uniform sampler2D shadow_map_direct;
//uniform sampler2D shadow_map_spot;
//uniform samplerCube shadow_map_point[NR_POINT_LIGHTS];
//
//
//
//
//
//
//
//void main()
//{
////    vec3 norm;
////    vec3 view_direction;
////    vec3 fragment_position;
////    vec2 texCoords;
////    mat3 TBN;
////
////
////    //vec3 ad = (1.0f,1.0f,1.0f);
////    //g_position = ad;
////    //g_normal = ad;
////    //g_albedo_spec.rgb = ad;
////    //g_albedo_spec.a = 1.0f;
////
////    // store the fragment position vector in the first gbuffer texture
////    //g_position = vec4(fs_in.frag_pos, 1.0f);
////    g_position = fs_in.frag_pos;
////
////
////    // also store the per-fragment normals into the gbuffer
////    g_normal = texture(material.normal_map1, fs_in.tex_coords).rgb;
////    g_normal = normalize(g_normal * 2.0f - 1.0f);
////
////
////    // and the diffuse per-fragment color
////    g_albedo_spec.rgb = texture(material.texture_diffuse1, fs_in.tex_coords).rgb;
////    // store specular intensity in gAlbedoSpec's alpha component
////    g_albedo_spec.a = texture(material.texture_specular1, fs_in.tex_coords).r;
//
//
//
//
//        g_position = fs_in.frag_pos;
//    // also store the per-fragment normals into the gbuffer
//    g_normal = normalize(fs_in.normal);
//    // and the diffuse per-fragment color
//    g_albedo_spec.rgb = vec3(0.95,0.95,0.95);
//    //g_albedo_spec.rgb = texture(material.texture_diffuse1, fs_in.tex_coords).rgb;
//    // store specular intensity in gAlbedoSpec's alpha component
//    g_albedo_spec.a = texture(material.texture_specular1, fs_in.tex_coords).r;
//}
//















#version 330 core
layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo_spec;

in VS_OUT{
	vec3 frag_pos;
	vec2 tex_coords;
	vec3 normal;
} vs_in;

struct Material{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
};

uniform Material material;

void main()
{    



    // store the fragment position vector in the first gbuffer texture
    //g_position = vec4(vs_in.frag_pos, 1.0f);
    g_position.xyz = vs_in.frag_pos;
    //g_position.a = 100.0f - vs_in.frag_pos.z;
    // also store the per-fragment normals into the gbuffer
    g_normal = normalize(vs_in.normal);
    // and the diffuse per-fragment color
    g_albedo_spec.rgb = texture(material.texture_diffuse1, vs_in.tex_coords).rgb;
    //g_albedo_spec.rgb = vec3(0.95);
    // store specular intensity in gAlbedoSpec's alpha component
    g_albedo_spec.a = texture(material.texture_specular1, vs_in.tex_coords).r;
}