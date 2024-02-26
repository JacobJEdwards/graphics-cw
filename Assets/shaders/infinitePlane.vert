#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float color;

void main() {
    float c = (
		int(round(aPos.x * 5.0)) +
		int(round(aPos.y * 5.0))
	) % 2;

    color = c;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}