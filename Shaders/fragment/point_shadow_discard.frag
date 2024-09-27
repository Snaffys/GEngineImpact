#version 330 core
out vec4 frag_color;

in GS_OUT{
    vec4 frag_pos;
    vec2 tex_coords;
} gs_in;

layout (std140) uniform pointLight{
    vec3 light_pos;
    float far_plane;
};

uniform sampler2D texture1;

void main()
{    

    float light_distance = length(gs_in.frag_pos.xyz - light_pos);

    light_distance = light_distance / far_plane;

    gl_FragDepth = light_distance;

    vec4 texColor = texture(texture1, gs_in.tex_coords);
    if(texColor.a < 0.1)
        discard;
}