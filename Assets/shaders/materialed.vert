#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 model;

#include "matrices.glsl"

void main() {
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = matrices.projection * matrices.view * model * vec4(aPos, 1.0);
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.Tangent = mat3(transpose(inverse(model))) * aTangent;
    vs_out.Bitangent = mat3(transpose(inverse(model))) * aBitangent;
    vs_out.FragPosLightSpace = matrices.lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
}