#version 410 core

layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

#include "matrices.glsl"

void main()
{
    gl_Position = matrices.projection * matrices.view * model * vec4(aPos, 1.0);
}