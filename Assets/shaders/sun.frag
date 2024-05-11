#version 410 core
out vec4 FragColor;
in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    sampler2D texture_ambient1;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    float shininess;
};

struct Light {
    vec4 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

const float glowIntensity = 0.8;

uniform Material material;
uniform Light sun;

void main() {
    vec4 texColor = texture(material.texture_diffuse1, TexCoords);

    texColor.rgb *= 2.0;

    vec3 glow = glowIntensity * sun.diffuse * smoothstep(0.2, 0.0, length(TexCoords - 0.5));

    texColor += vec4(glow, 1.0);

    FragColor = texColor;

    FragColor.a = 0.8;
}
