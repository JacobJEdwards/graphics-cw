#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec3 FragPosWorld;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
    FragPos = aPos;
    FragPosWorld = vec3(view * projection * vec4(FragPos, 1.0));

    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_Position.z = gl_Position.w;
}

