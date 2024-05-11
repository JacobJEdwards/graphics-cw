#version 410 core

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


in VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    vec3 Tangent;
    vec3 Bitangent;
} fs_in;

#define MAX_SAMPLES 100
#define STEP_SIZE 0.1
#define DECAY_FACTOR 0.95
#define EXPOSURE 0.12
#define MAX_DEPTH 100.0

uniform Light sun;
uniform vec3 viewPos;
uniform vec3 viewDir;

out vec4 FragColor;

float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

float noise(vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f * f * (3.0 - 2.0 * f);

    float n = p.x + p.y * 157.0 + 113.0 * p.z;
    return mix(mix(mix(hash(n + 0.0), hash(n + 1.0), f.x),
    mix(hash(n + 157.0), hash(n + 158.0), f.x), f.y),
    mix(mix(hash(n + 113.0), hash(n + 114.0), f.x),
    mix(hash(n + 270.0), hash(n + 271.0), f.x), f.y), f.z);
}


float fade(float t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}


float getDensity(vec3 samplePoint) {
    return noise(samplePoint) * 0.5;
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
    vec3 lightDir = normalize(sun.position - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 rayDir = normalize(viewPos - sun.position);

    float visibility = sampleCloudVolume(fs_in.FragPos, rayDir);

    vec3 godrays = sun.diffuse * visibility * fade(1.0 - visibility) * 0.5;

    vec3 finalColor = baseColor + godrays;

    float sunHeight = sun.position.y;
    float sunFactor = clamp(sunHeight, 0.3, 1.0);

    finalColor *= sunFactor;

    FragColor = vec4(finalColor, 0.8);


}
