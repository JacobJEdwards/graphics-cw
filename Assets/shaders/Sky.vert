#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec3 FragPosWorld;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
    FragPos = aPos;
    FragPosWorld = vec3(view * projection * vec4(FragPos, 1.0));

    Normal = mat3(transpose(inverse(model))) * aPos;

    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_Position.z = gl_Position.w;

    TexCoords = aPos.xy;

}

