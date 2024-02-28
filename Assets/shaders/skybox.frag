#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

uniform float intensity;

void main() {
    vec3 color = texture(skybox, TexCoords).rgb;
    float factor = min(max(0.2, intensity), 1.2);
    FragColor = vec4(color * factor, 1.0);
    // FragColor = texture(skybox, TexCoords);
}