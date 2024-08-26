#version 330 core
out vec4 frag_color;

in VS_OUT{
    vec2 tex_coords;
} fs_in;

uniform sampler2D texture1;

void main()
{    
    vec4 texColor = texture(texture1, fs_in.tex_coords);
    if(texColor.a < 0.1)
        discard;
    frag_color = texColor;
}