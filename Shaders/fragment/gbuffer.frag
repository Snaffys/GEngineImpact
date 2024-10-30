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

float linearize_depth(float d)
{
    float z_n = 2.0 * d - 1.0;
    return 2.0 * 0.1 * 100.0 / (100.0 + 0.1 - z_n * (100.0 - 0.1));
}

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    //g_position = vec4(vs_in.frag_pos, 1.0f);
    g_position.xyz = vs_in.frag_pos;
    //g_position.a = 100.0f - linearize_depth(vs_in.frag_pos.z);
    // also store the per-fragment normals into the gbuffer
    g_normal = normalize(vs_in.normal);
    // and the diffuse per-fragment color
    g_albedo_spec.rgb = texture(material.texture_diffuse1, vs_in.tex_coords).rgb;
   // g_albedo_spec.rgb = vec3(0.95);
    // store specular intensity in gAlbedoSpec's alpha component
    g_albedo_spec.a = texture(material.texture_specular1, vs_in.tex_coords).r;
}
