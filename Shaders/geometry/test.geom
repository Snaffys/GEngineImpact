
#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 tex_coords;
	vec3 normal;
	vec3 frag_pos;
	vec4 frag_pos_light;
} gs_in[];

out GS_OUT{
    vec2 tex_coords;
    vec3 normal;
	vec3 frag_pos;
	vec4 frag_pos_light;
} gs_out;

uniform float time;

vec4 Explode(vec4 position, vec3 normal);
vec3 GetNormal();


void main() {   
    vec3 normal = GetNormal();
    //gl_Position = gl_in[0].gl_Position;
    gl_Position = Explode(gl_in[0].gl_Position, normal);
    gs_out.tex_coords       = gs_in[0].tex_coords;
    gs_out.normal           = gs_in[0].normal;
    gs_out.frag_pos         = gs_in[0].frag_pos;
    gs_out.frag_pos_light   = gs_in[0].frag_pos_light;

    EmitVertex();
    //gl_Position = gl_in[1].gl_Position;
    gl_Position = Explode(gl_in[1].gl_Position, normal);
    gs_out.tex_coords       = gs_in[1].tex_coords;
    gs_out.normal           = gs_in[1].normal;
    gs_out.frag_pos         = gs_in[1].frag_pos;
    gs_out.frag_pos_light   = gs_in[1].frag_pos_light;
    EmitVertex();
    //gl_Position = gl_in[2].gl_Position;
    gl_Position = Explode(gl_in[2].gl_Position, normal);
    gs_out.tex_coords       = gs_in[2].tex_coords;
    gs_out.normal           = gs_in[2].normal;
    gs_out.frag_pos         = gs_in[2].frag_pos;
    gs_out.frag_pos_light   = gs_in[2].frag_pos_light;
    EmitVertex();
    EndPrimitive();
}


vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 Explode(vec4 position, vec3 normal)
{
    float magnitude = 0.5;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}