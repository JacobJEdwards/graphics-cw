#version 410 core

in VS_OUT {
    vec3 FragPos;
    vec3 FragPosWorld;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#include "noise.frag"
#include "lights.glsl"
#include "camera.glsl"

uniform vec3 horizonColor = vec3(0.5, 0.6, 0.7);
uniform vec3 zenithColor = vec3(0.4, 0.7, 1.0);
uniform vec4 cloudColor = vec4(1.0, 1.0, 1.0, 0.75);

uniform float cloudCover = 0.2;
uniform float cloudSharpness = 0.8;
uniform float cloudDensity = 0.5;
uniform float cloudScale = 0.02;
uniform float cloudSpeed = 0.01;

uniform float sunSize = 8.0;
uniform float sunSharpness = 30.0;
uniform float sunIntensity =  1.5;

uniform float horizonHeight = 0.2;
uniform float horizonSharpness = 0.9;

uniform float zenithHeight = 0.6;
uniform float zenithSharpness = 0.9;

uniform vec3 skyColor = vec3(0.5, 0.6, 0.7);

uniform float time = 0.0;

// stars
uniform float starDensity = 0.00001;
uniform float starSharpness = 1.0;
uniform float starSize = 0.01;

void main() {
    vec3 viewDir = normalize(fs_in.FragPos);

    float sunHeightFactor = clamp(lights.sun.direction.y, 0.0, 1.0);

    vec3 finalSkyColor = mix(zenithColor, horizonColor, sunHeightFactor);
    finalSkyColor += sunHeightFactor * vec3(1.0, 0.6, 0.3);

    float sunAmount = pow(max(0.0, dot(lights.sun.direction, viewDir)), sunSharpness);
    vec3 finalSun = lights.sun.diffuse * sunAmount * sunIntensity;

    float cloudAmount = pow(max(0.0, 1.0 - fs_in.FragPos.y), cloudSharpness);
    float cloudNoise = fbm3d(fs_in.FragPos * cloudScale + vec3(0.0, time * cloudSpeed, 0.0));
    float cloudDensityFactor = cloudDensity * cloudCover;
    vec4 cloud = cloudColor * cloudAmount * cloudNoise * cloudDensityFactor;

    float zenithFactor = pow(max(0.0, fs_in.FragPos.y), zenithSharpness);
    finalSkyColor = mix(finalSkyColor, zenithColor, zenithFactor);

    finalSkyColor = mix(finalSkyColor, vec3(0.001, 0.001, 0.001), 1.0 - sunHeightFactor);
    cloud = mix(cloud, vec4(0.0, 0.0, 0.0, 0.0), 1.0 - sunHeightFactor);

    float starProb = step(hash((fs_in.FragPosWorld.x + fs_in.FragPosWorld.y + fs_in.FragPosWorld.z) * 0.1),
    starDensity);

    vec3 stars = vec3(1.0, 1.0, 1.0) * starProb;

    stars = mix(vec3(0.0, 0.0, 0.0), stars, 1.0 - sunHeightFactor);

    finalSkyColor += finalSun;

    finalSkyColor = mix(finalSkyColor, cloud.rgb, cloud.a);

    float lightFactor = max(dot(lights.sun.direction, vec3(0.0, 1.0, 0.0)), 0.0);
    vec3 starsFinal = mix(stars, vec3(0.0, 0.0, 0.0), lightFactor * 2.0);
    finalSkyColor += starsFinal;

    FragColor = vec4(finalSkyColor, 1.0);
}