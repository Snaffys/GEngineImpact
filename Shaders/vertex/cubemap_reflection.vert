#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 3) in mat4 instanceMatrix;

out vec3 normal;
out vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    normal = mat3(transpose(inverse(instanceMatrix))) * a_normal;
    position = vec3(instanceMatrix * vec4(a_pos, 1.0));
    gl_Position = projection * view * vec4(position, 1.0);
}