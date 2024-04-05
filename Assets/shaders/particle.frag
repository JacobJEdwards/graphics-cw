#version 410 core

out vec4 FragColor;

uniform vec4 color;
uniform float life;

// in vec4 fColor;
// in float fLife;

void main() {
    FragColor = color * clamp(color, 0.0, 1.0);
    FragColor.a = clamp(life, 0.0, 1.0);
}
