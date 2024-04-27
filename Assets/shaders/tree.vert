#version 410 core

layout (location = 0) in vec3 position;
layout (location = 2) in vec2 texCoords;
layout (location = 6) in mat4 instanceModelMatrix;

out VS_OUT {
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * instanceModelMatrix * vec4(position, 1.0);;
    vs_out.TexCoords = texCoords;
}

