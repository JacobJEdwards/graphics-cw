#version 410 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} fs_in;

uniform Material material;

#include "lighting.frag"
#include "camera.glsl"

uniform vec3 color;

out vec4 FragColor;

void main() {
    vec3 result = calculateLighting(fs_in.FragPos, fs_in.Normal, camera.position, material.diffuse.rgb * color, material
    .shininess);

    FragColor = vec4(result, 1.0);

    if (FragColor.a < 0.1) {
        discard;
    }
}



