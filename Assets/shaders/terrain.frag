#version 410 core

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} fs_in;

#include "lighting.frag"
#include "noise.frag"
#include "camera.glsl"

uniform vec3 pathedPoints[512];
uniform int pathedPointsCount;
uniform float pathedPointsRadius = 100.0;
uniform float pathDarkness = 0.5;

const vec3 grassColor = vec3(0.2, 0.8, 0.2);
const vec3 pathColor = vec3(0.2, 0.1, 0.0);


bool isOnPath(vec3 fragPos, vec3 pathedPoint, vec3 nextPathedPoint) {
    float distToSegment = distance(nextPathedPoint, pathedPoint);
    float distToStart = distance(fragPos, pathedPoint);
    float distToEnd = distance(fragPos, nextPathedPoint);

    float dotProduct = dot(normalize(nextPathedPoint - pathedPoint), normalize(fragPos - pathedPoint));

    return (dotProduct >= 0.0 && dotProduct <= distToSegment && distToStart <= distToSegment && distToEnd <= distToSegment);
}

void main() {
    vec3 color = grassColor;

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    vec3 lighting = calculateLighting(fs_in.FragPos, fs_in.Normal, camera.position, color, 128.0) * 0.8;

    float noiseVal = fbm2d(fs_in.TexCoords * 10.0);
    lighting += vec3(noiseVal * 0.1);

    for (int i = 0; i < pathedPointsCount; i++) {
        vec3 pathedPoint = pathedPoints[i];
        vec3 nextPathedPoint = pathedPoints[(i + 1) % pathedPointsCount];

        if (isOnPath(fs_in.FragPos, pathedPoint, nextPathedPoint)) {
            lighting = mix(lighting, pathColor, pathDarkness);
            break;
        }
    }

    FragColor = vec4(lighting, 1.0);
    // FragColor = vec4(lights.sun.specular, 1.0);
}
