#version 410 core
layout (location = 0) in vec3 aPos;

out VS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 model;

#include "matrices.glsl"

void main() {
    mat4 newView = mat4(mat3(matrices.view));

    vs_out.FragPos = aPos;
    vs_out.FragPosWorld = vec3(newView * matrices.projection * vec4(vs_out.FragPos, 1.0));

    vs_out.Normal = mat3(transpose(inverse(model))) * aPos;

    gl_Position = matrices.projection * newView * vec4(aPos, 1.0);
    gl_Position.z = gl_Position.w;

    vs_out.TexCoords = aPos.xy;
    vs_out.FragPosLightSpace = matrices.lightSpaceMatrix * model * vec4(aPos, 1.0);
}

