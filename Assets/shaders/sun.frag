#version 330 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D texture1;

void main() {
    color = texture(texture1, TexCoords);
    if (color.a < 0.1) {
        discard;
    }
}