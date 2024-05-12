#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    vec4 diffuse;
    float shininess;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 UnalteredNormal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} fs_in;

#include "lighting.frag"
#include "camera.glsl"

uniform sampler2D damageTexture;
uniform Material material;
uniform float damage = 0.0;


void main() {
    vec4 texColor = texture(material.texture_diffuse1, fs_in.TexCoords);

    vec3 result = calculateLighting(fs_in.FragPos, fs_in.Normal, camera.position, texColor.xyz, material.shininess);

    float damageFactor = texture(damageTexture, fs_in.TexCoords).r;
    result *= mix(1.0, damageFactor, damage);

    FragColor = vec4(lights.sun.ambient, 1.0);

}