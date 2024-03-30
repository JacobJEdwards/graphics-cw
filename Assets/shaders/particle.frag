#version 410 core

in vec3 FragPos;
in vec2 TexCoords;


uniform vec4 color;
uniform float life;

out vec4 FragColor;

void main() {
    vec4 texColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float alpha = 1.0f;
    if (life < 0.5f) {
        alpha = life / 0.5f;
    }
    FragColor = vec4(texColor.rgb * color.rgb, alpha);
}

