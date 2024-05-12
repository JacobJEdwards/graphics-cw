#version 410 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec3 FragPos;
    vec4 FragPosLightSpace;
} fs_in;

#include "lighting.frag"

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


const float glowIntensity = 0.8;

uniform Material material;

void main() {
    vec4 texColor = texture(material.texture_diffuse1, fs_in.TexCoords);

    texColor.rgb *= 2.0;

    vec3 glow = glowIntensity * lights.sun.diffuse * smoothstep(0.2, 0.0, length(fs_in.TexCoords - 0.5));

    texColor += vec4(glow, 1.0);

    FragColor = texColor;

    FragColor.a = 0.8;
}
