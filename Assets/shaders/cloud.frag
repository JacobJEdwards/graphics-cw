#version 410 core

in VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} fs_in;

#include "lighting.frag"
#include "noise.frag"
#include "camera.glsl"

#define MAX_SAMPLES 100
#define STEP_SIZE 0.1
#define DECAY_FACTOR 0.95
#define EXPOSURE 0.12
#define MAX_DEPTH 100.0

out vec4 FragColor;

float getDensity(vec3 samplePoint) {
    return noise3d(samplePoint) * 0.5;
}

float sampleCloudVolume(vec3 rayOrigin, vec3 rayDir) {
    float sampleDepth = 0.0;
    float visibility = 1.0;

    for (int i = 0; i < MAX_SAMPLES; ++i) {
        vec3 samplePoint = rayOrigin + rayDir * sampleDepth;
        float density = getDensity(samplePoint);

        visibility *= exp(-density * STEP_SIZE);

        sampleDepth += STEP_SIZE;

        if (visibility < 0.001 || sampleDepth > MAX_DEPTH) {
            break;
        }
    }

    return visibility;
}

void main() {
    vec3 baseColor = vec3(0.5, 0.6, 0.7);
    // vec3 lighting = calculateDirectionalLight(lights.sun, fs_in.Normal, camera.position, baseColor, 32.0, 0.0) * 2.0;


    float visibility = sampleCloudVolume(fs_in.FragPos, lights.sun.direction);

    vec3 godrays = lights.sun.diffuse * visibility * fade(1.0 - visibility) * 0.5;

    vec3 finalColor = baseColor + godrays;
    finalColor = mix(finalColor, lights.sun.ambient, 0.2);

    // if direction of light is below horizon, make it darker
    float lightFactor = max(dot(lights.sun.direction, vec3(0.0, 1.0, 0.0)), 0.1);
    finalColor *= lightFactor;

    FragColor = vec4(finalColor, 0.8);


}
