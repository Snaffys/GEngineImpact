#version 330 core

// output
layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

in VS_OUT {
    vec3 tex_coords;
} fs_in;

uniform samplerCube cubemap;

void main(){
    frag_color = texture(cubemap, fs_in.tex_coords);
    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bright_color = vec4(frag_color.rgb, 1.0);
    else
        bright_color = vec4(0.0, 0.0, 0.0, 1.0);
}