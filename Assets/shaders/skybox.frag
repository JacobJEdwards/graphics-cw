#version 410 core
out vec4 FragColor;

in vec3 TexCoords;
in vec3 FragPos;

uniform samplerCube skybox;

uniform float intensity;

struct Fog {
    float density;
    vec3 color;
};


void main() {
    Fog fog;
    fog.density = 1;
    fog.color = vec3(0.5, 0.5, 0.5);

    vec3 viewDir = normalize(FragPos);
    float fogFactor = exp(-fog.density * fog.density * length(viewDir));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 color = texture(skybox, TexCoords).rgb;
    color = mix(fog.color, color, fogFactor);

    float factor = min(max(0.2, intensity), 1.2);
    color *= factor;

    FragColor = vec4(color, 1.0);
}