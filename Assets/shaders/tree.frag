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
#include "camera.glsl"

out vec4 FragColor;

void main() {
    vec4 texColor = texture(material.texture_diffuse1, fs_in.TexCoords);

    vec3 normal = normalize(fs_in.Normal);

    // vec3 lighting = calculateLightingBasic(fs_in.FragPos, normal, camera.position, texColor.rgb, material.shininess);
    vec3 finalColor = texColor.rgb;

    finalColor = mix(finalColor, lights.sun.ambient, 0.2);

    // if direction of light is below horizon, make it darker
    float lightFactor = max(dot(lights.sun.direction, vec3(0.0, 1.0, 0.0)), 0.1);
    finalColor *= lightFactor;

    FragColor = vec4(finalColor, texColor.a);

    if (FragColor.a < 0.2) {
        discard;
    }
}
