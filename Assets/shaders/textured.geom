// geom shader to explode vertices
#version 410 core

/*
 * https://learnopengl.com/Advanced-OpenGL/Geometry-Shader
 */

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
    vec3 UnalteredNormal;
} gs_in[];

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
    vec3 UnalteredNormal;
} fs_in;

const float magnitude = 50.0;

vec4 explode(vec4 position, vec3 normal) {
    vec3 direction = normal * time * magnitude;
    return position + vec4(direction, 0.0);
}

void main() {
    vec3 normal = normalize(cross(gs_in[0].UnalteredNormal, gs_in[1].UnalteredNormal));

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
