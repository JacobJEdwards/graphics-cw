#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 FragPos;

uniform mat4 model;

#include "matrices.glsl"

void main() {
    FragPos = vec3(model * vec4(position, 1.0));
    gl_Position = matrices.projection * matrices.view * vec4(FragPos, 1.0);
}
