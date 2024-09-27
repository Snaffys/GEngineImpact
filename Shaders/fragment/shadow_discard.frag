#version 330 core
out vec4 frag_color;

in VS_OUT{
    vec2 tex_coords;
} fs_in;

uniform sampler2D texture1;

void main()
{    
    vec4 tex_color = texture(texture1, fs_in.tex_coords);
    if(tex_color.a < 0.1)
        discard;
}