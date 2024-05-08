#version 410 core
out vec4 FragColor;
in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    sampler2D texture_ambient1;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    vec4 diffuse;
    float shininess;
};

const float glowIntensity = 0.8;

uniform Material material;

void main() {
    vec4 texColor = texture(material.texture_diffuse1, TexCoords);

    texColor.rgb *= 2.0;

    vec4 glowColor = vec4(1.0, 0.8, 0.2, 1.0);
    vec4 glow = glowIntensity * glowColor * smoothstep(0.2, 0.0, length(TexCoords - 0.5));

    texColor += glow;

    FragColor = texColor;
}
