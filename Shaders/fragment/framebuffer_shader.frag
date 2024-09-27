#version 330 core
out vec4 frag_color;

in VS_OUT{
    vec2 tex_coords;
} fs_in;

uniform sampler2D screen_texture;
uniform sampler2D bloom_blur;
uniform float exposure;

const float offset = 1.0/900.0;

void main(){
	float gamma = 2.2;
    // Default output
    vec3 hdr_color = texture(screen_texture, fs_in.tex_coords).rgb;
    vec3 bloom_color = texture(bloom_blur, fs_in.tex_coords).rgb;
    hdr_color += bloom_color;
    vec3 mapped = vec3(1.0f) - exp(-hdr_color * exposure);
    mapped = pow(mapped, vec3(1.0f/gamma));
    
    

	//// Inversion
	//mapped = vec3(1.0 - mapped);

	//// Grayscale
    //float average = 0.2126 * mapped.r + 0.7152 * mapped.g + 0.0722 * mapped.b;
    //mapped = vec3(average, average, average);
    
    //// Kernel
    //vec2 offsets[9] = vec2[](
	//    vec2(-offset,  offset), // top-left
    //    vec2( 0.0f,    offset), // top-center
    //    vec2( offset,  offset), // top-right
    //    vec2(-offset,  0.0f),   // center-left
    //    vec2( 0.0f,    0.0f),   // center-center
    //    vec2( offset,  0.0f),   // center-right
    //    vec2(-offset, -offset), // bottom-left
    //    vec2( 0.0f,   -offset), // bottom-center
    //    vec2( offset, -offset)  // bottom-right  
    //);
    //float kernel[9] = float[](
    //   1, 1, 1,
    //   1, -8, 1,
    //   1, 1, 1
    //);

	//float kernel[9] = float[](
    //    1.0 / 16, 2.0 / 16, 1.0 / 16,
    //    2.0 / 16, 4.0 / 16, 2.0 / 16,
    //    1.0 / 16, 2.0 / 16, 1.0 / 16  
    //);
    
    //vec3 sampleTex[9];
    //for(int i = 0; i < 9; i++)
    //    sampleTex[i] = texture(screen_texture, fs_in.tex_coords.st + offsets[i]).rgb;
    //for(int i = 0; i < 9; i++)
    //    mapped += sampleTex[i] * kernel[i];

	frag_color = vec4(mapped, 1.0f);
}


//#version 330 core
//out vec4 frag_color;
//
//in vec2 tex_coords;
//
//uniform sampler2D depthMap;
//uniform float near_plane;
//uniform float far_plane;
//
//// required when using a perspective projection matrix
//float LinearizeDepth(float depth)
//{
//    float z = depth * 2.0 - 1.0; // Back to NDC 
//    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
//}
//
//void main()
//{     
//    float depthValue = texture(depthMap, tex_coords).r;
//    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
//    frag_color = vec4(vec3(depthValue), 1.0); // orthographic
//}