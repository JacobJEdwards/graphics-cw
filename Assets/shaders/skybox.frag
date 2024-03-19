#version 410 core
out vec4 FragColor;

in vec3 TexCoords;
in vec3 FragPos;

uniform samplerCube skybox;

uniform float intensity;

void main() {
    vec3 color = texture(skybox, TexCoords).rgb;

    float factor = min(max(0.2, intensity), 1.0);
    color *= factor;

    FragColor = vec4(color, 1.0);
}
