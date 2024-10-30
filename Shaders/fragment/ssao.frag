#version 330 core

layout (location = 0) out float frag_color;

in VS_OUT{
    vec2 tex_coords;
} fs_in;

uniform float width;
uniform float height;

uniform sampler2D g_position;
uniform sampler2D g_normal;

uniform sampler2D tex_noise; // 4x4 (GL_REPEAT) texture with vectors rotation information in it
vec2 noise_scale = vec2(width/4.0, height/4.0);

uniform vec3 samples[64];
uniform mat4 projection;

uniform float far_plane;

const int kernel_size = 64;
const float radius = 0.5;   // hemisphere radius
float bias = 0.025;



// counts depth samples inside geometry and devises it by sum of all samples inside hemisphere
void main(){
    vec3 frag_pos = texture(g_position, fs_in.tex_coords).xyz;
    vec3 normal = normalize(texture(g_normal, fs_in.tex_coords).rgb);
    vec3 random_vec = normalize(texture(tex_noise, fs_in.tex_coords * noise_scale).xyz);

    // get TBN matrix to transfom all samples vectors from tangent to view space
    vec3 tangent = normalize(random_vec - normal * dot(random_vec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);  

    float occlusion = 0.0f;

    for(int i = 0; i < kernel_size; i++){
        vec3 sample_pos = TBN * samples[i]; // transform sample to view space
        sample_pos = frag_pos.xyz + sample_pos * radius;

        // transforms sample_pos to clip space
        vec4 offset = vec4(sample_pos, 1.0);
        offset = projection * offset;   // from view to clip space
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0  

        //float sample_depth = -(far_plane - texture(g_position, offset.xy).w); // get depth value
        float sample_depth = texture(g_position, offset.xy).z; // get depth value of sample in hemisphere
        if(sample_depth == 0)
        occlusion += 0;
        else{

        
        float range_check = smoothstep(0.0, 1.0, radius / abs(frag_pos.z - sample_depth)); // removes outline if other objects too far away
        occlusion += (sample_depth >= sample_pos.z + bias ? 1.0 : 0.0) * range_check; // if sample closer than geometry point, add 1 to amount of samples inside geometry
        }
    }
    occlusion = 1.0 - (occlusion / kernel_size);    // shading efficiency
    frag_color = occlusion;
}