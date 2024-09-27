#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 5) in mat4 instanceMatrix;

out VS_OUT{
    vec3 normal;
    vec3 position;
} vs_out;

layout (std140) uniform matrices{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main(){
    vs_out.normal = mat3(transpose(inverse(instanceMatrix))) * a_normal;
    vs_out.position = vec3(instanceMatrix * vec4(a_pos, 1.0));
    gl_Position = projection * view * vec4(vs_out.position, 1.0);
}