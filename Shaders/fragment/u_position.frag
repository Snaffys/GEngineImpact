#version 330 core

in vec3 vert_color;		// gets color from vertex shader

out vec4 frag_color;	// retutns color

void main()
{
    frag_color = vec4(vert_color, 1.0);	// color appropriation
}