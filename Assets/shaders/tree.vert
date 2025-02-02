#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

layout (location = 6) in mat4 instanceModelMatrix;

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} vs_out;

#include "matrices.glsl"

void main() {
    gl_Position = matrices.projection * matrices.view * instanceModelMatrix * vec4(position, 1.0);;
    vs_out.TexCoords = texCoords;
    vs_out.FragPos = vec3(instanceModelMatrix * vec4(position, 1.0));
    vs_out.Normal = mat3(transpose(inverse(instanceModelMatrix))) * normal;
    vs_out.Tangent = mat3(transpose(inverse(instanceModelMatrix))) * tangent;
    vs_out.Bitangent = mat3(transpose(inverse(instanceModelMatrix))) * bitangent;
    vs_out.FragPosLightSpace = matrices.lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
}

