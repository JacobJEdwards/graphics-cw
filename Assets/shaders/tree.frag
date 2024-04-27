#version 410 core

in VS_OUT {
    vec2 TexCoords;
} fs_in;

uniform sampler2D texture_diffuse1;

out vec4 FragColor;

void main() {
    FragColor = texture(texture_diffuse1, fs_in.TexCoords);
    if (FragColor.a < 0.1) {
        discard;
    }
}
