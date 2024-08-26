#version 330 core
out vec4 frag_color;

in VS_OUT{
    vec3 normal;
    vec3 position; 
} fs_in;

uniform vec3 camera_pos;

uniform samplerCube cubemap;

void main()
{   
    // reflection
    //vec3 I = normalize(fs_in.position - camera_pos);
    //vec3 R = reflect(I, normalize(fs_in.normal));
    //frag_color = vec4(texture(cubemap, R).rgb, 1.0);

    // refraction
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(fs_in.position - camera_pos);
    vec3 R = refract(I, normalize(fs_in.normal), ratio);
    frag_color = vec4(texture(cubemap, R).rgb, 1.0);
}
