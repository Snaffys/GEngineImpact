#version 330 core
out vec4 frag_color;

in vec3 normal;
in vec3 position;

uniform vec3 camera_pos;
uniform samplerCube cubemap;

void main()
{   
    // reflection
//    vec3 I = normalize(position - camera_pos);
//    vec3 R = reflect(I, normalize(normal));
//    frag_color = vec4(texture(cubemap, R).rgb, 1.0);

    // refraction
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(position - camera_pos);
    vec3 R = refract(I, normalize(normal), ratio);
    frag_color = vec4(texture(cubemap, R).rgb, 1.0);
}
