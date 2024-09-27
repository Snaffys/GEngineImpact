
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT{
	mat4 projection;
	vec3 normal;
} gs_in[];

void DrawNormalDirection(int i);

void main() {   
    DrawNormalDirection(0);
    DrawNormalDirection(1);
    DrawNormalDirection(2);
}


void DrawNormalDirection(int i){
    gl_Position = gs_in[0].projection * gl_in[i].gl_Position;
    EmitVertex();

    gl_Position = gs_in[0].projection * (gl_in[i].gl_Position + vec4(gs_in[i].normal, 0.0) * 0.01f);
    EmitVertex();
    EndPrimitive();
}
