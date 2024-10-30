#version 330 core
out float frag_color;

in VS_OUT{
    vec2 tex_coords;
} fs_in;

uniform sampler2D ssao;

void main(){
    vec2 texel_size = 1.0 / vec2(textureSize(ssao, 0));

    float result = 0.0f;

    for(int x = -2; x < 2; x++){
        for(int y = -2; y < 2; y++){
            vec2 offset = vec2(float(x), float(y)) * texel_size;
            result += texture(ssao, fs_in.tex_coords + offset).r;
        }
    }

    frag_color = result / (4.0f * 4.0f);
}