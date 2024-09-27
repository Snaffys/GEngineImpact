#version 330 core

#define size_of_weight 5

out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D image;
uniform bool horizontal;
uniform float weight[size_of_weight] = {0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162};

void main(){
    vec2 tex_offset = 1.0f / textureSize(image, 0);
    vec3 result = texture(image, tex_coords).rgb * weight[0];
    if(horizontal){
        for(int i = 1; i < size_of_weight; i++){
            result += texture(image, tex_coords + vec2(tex_offset.x * i, 0.0f)).rgb * weight[i];
            result += texture(image, tex_coords - vec2(tex_offset.x * i, 0.0f)).rgb * weight[i];
        }
    }
    else{
        for(int i = 1; i < size_of_weight; i++){
            result += texture(image, tex_coords + vec2(0.0f, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, tex_coords - vec2(0.0f, tex_offset.y * i)).rgb * weight[i];
        }
    }
    frag_color = vec4(result, 1.0f);
}
