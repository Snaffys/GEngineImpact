#version 330 core
out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D texture1;

void main()
{    
    vec4 texColor = texture(texture1, tex_coords);
    frag_color = texColor;
}