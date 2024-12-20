#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 Normal;
    vec3 FragPos;
    vec2 TexCoords;
} vs_out;

#include "matrices.glsl"

void main()
{
    gl_Position = matrices.projection * matrices.view * vec4(aPos, 1.0);

    vs_out.Normal = aNormal;
    vs_out.FragPos = vec3(vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;

}
