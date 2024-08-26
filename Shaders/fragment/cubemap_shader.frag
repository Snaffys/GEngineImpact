#version 330 core
out vec4 frag_color;

in VS_OUT {
    vec3 tex_coords;
} fs_in;

uniform samplerCube cubemap;

void main(){
    frag_color = texture(cubemap, fs_in.tex_coords);
}