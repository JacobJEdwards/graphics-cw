// geom shader to explode vertices
#version 410 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform float time = 0.0;


// pass through to fragment shader
in VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} gs_in[];

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} fs_in;

vec4 explode(vec4 position, vec3 normal) {
    if (time == 0.0) {
        return position;
    }

    float magnitude = 50.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 0.0);
}

void main() {
    vec3 normal = normalize(cross(gs_in[0].Normal, gs_in[1].Normal));

    for (int i = 0; i < 3; i++) {
        gl_Position = explode(gl_in[i].gl_Position, normal);
        fs_in.TexCoords = gs_in[i].TexCoords;
        fs_in.FragPos = gs_in[i].FragPos;
        fs_in.Normal = gs_in[i].Normal;
        fs_in.Tangent = gs_in[i].Tangent;
        fs_in.Bitangent = gs_in[i].Bitangent;
        fs_in.FragPosLightSpace = gs_in[i].FragPosLightSpace;
        EmitVertex();
    }

    EndPrimitive();
}
