#version 410 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    vec4 ambient;
    vec4 specular;
    vec4 emmisive;
    vec4 diffuse;
    float shininess;
};

in VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} fs_in;

uniform Material material;

#include "lights.glsl"

out vec4 FragColor;

void main() {
    vec4 texColor = texture(material.texture_diffuse1, fs_in.TexCoords);

    FragColor = texColor;

    if (FragColor.a < 0.2) {
        discard;
    }
}
