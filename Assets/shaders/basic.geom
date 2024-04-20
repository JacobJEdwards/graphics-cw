// geom shader to explode vertices
#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float explode = 0.1;

// pass through to fragment shader
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} gs_in[];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

void main() {
    for (int i = 0; i < gl_in.length(); i++) {
        vec3 p = gl_in[i].gl_Position.xyz;
        vec3 n = gs_in[i].Normal;
        gl_Position = gl_in[i].gl_Position + vec4(n * explode, 0.0);
        fs_in.FragPos = gl_Position.xyz;
        fs_in.Normal = n;
        fs_in.TexCoords = gs_in[i].TexCoords;
        EmitVertex();
    }
    EndPrimitive();
}
