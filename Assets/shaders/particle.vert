#version 410 core

layout(location = 0) in vec3 aPos;
/*
layout(location = 3) in vec3 newPos;
layout(location = 4) in vec3 velocity;
layout(location = 5) in vec4 color;
layout(location = 6) in float life;
layout(location = 7) in mat4 model;
*/

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

// out vec4 fColor;
// out float fLife;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // fColor = color;
    // fLife = life;
}
