#version 330 core
out vec4 frag_color;

in vec3 tex_coords;

uniform samplerCube cubemap;

void main(){
    //frag_color = vec4(1.0, 0.0, 0.0, 1.0);
    frag_color = texture(cubemap, tex_coords);
}