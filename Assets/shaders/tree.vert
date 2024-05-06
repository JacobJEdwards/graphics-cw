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
} vs_out;

uniform mat4 projection;
uniform mat4 view;

void main() {
    gl_Position = projection * view * instanceModelMatrix * vec4(position, 1.0);;
    vs_out.TexCoords = texCoords;
    vs_out.FragPos = vec3(instanceModelMatrix * vec4(position, 1.0));
    vs_out.Normal = mat3(transpose(inverse(instanceModelMatrix))) * normal;
    vs_out.Tangent = mat3(transpose(inverse(instanceModelMatrix))) * tangent;
    vs_out.Bitangent = mat3(transpose(inverse(instanceModelMatrix))) * bitangent;
}

