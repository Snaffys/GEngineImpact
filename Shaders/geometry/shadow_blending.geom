#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadow_matrices[6];

in VS_OUT{
    vec2 tex_coords;
} vs_out[];

out GS_OUT{
    vec4 frag_pos;
    vec2 tex_coords;
}gs_out;


void main(){
    for(int face = 0; face < 6; face++){
        gl_Layer = face;
        for(int i = 0; i < 3; i++){
            gs_out.frag_pos = gl_in[i].gl_Position;
            gl_Position = shadow_matrices[face] * gs_out.frag_pos;
            gs_out.tex_coords = vs_out[i].tex_coords;
            EmitVertex();
        }
        EndPrimitive();
    }
}