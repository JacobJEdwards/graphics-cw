#version 410 core

out vec4 FragColor;

uniform vec4 color;
uniform float life;

void main() {
    FragColor = color * exp(-life * 3.0);
    FragColor.a = clamp(life, 0.0, 1.0);
}
