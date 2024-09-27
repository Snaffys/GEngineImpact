#version 330 core
// output
layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

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

    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bright_color = vec4(frag_color.rgb, 1.0);
    else
        bright_color = vec4(0.0, 0.0, 0.0, 1.0);
}
