#version 410 core

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;
uniform sampler2D noiseTexture;
uniform sampler2D normalMap;

uniform vec3 viewPos;
uniform Light sun;

// points that have been pathed
uniform vec3 pathedPoints[256];
uniform int pathedPointsCount = 0;
uniform float pathedPointsRadius = 5.0;
uniform float pathDarkness = 0.5;

const vec3 grassColor = vec3(0.2, 0.8, 0.2);
const vec3 pathColor = vec3(0.2, 0.1, 0.0);


// noise funcs
float rand(vec2 co){
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0-2.0*u);

    float res = mix(
    mix(rand(ip), rand(ip + vec2(1.0, 0.0)), u.x),
    mix(rand(ip + vec2(0.0, 1.0)), rand(ip + vec2(1.0, 1.0)), u.x),
    u.y
    );
    return res * res;
}

float fbm(vec2 p) {
    float f = 0.0;
    f += 0.5000 * noise(p); p = p * 2.02;
    f += 0.2500 * noise(p); p = p * 2.03;
    f += 0.1250 * noise(p); p = p * 2.01;
    f += 0.0625 * noise(p);
    return f / 0.9375;
}

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.Normal);
    float bias = max(0.05 * (1.0 - dot(normal, sun.direction)), 0.005);

    float shadow = 0.0;
    float texelSize = 1.0 / textureSize(shadowMap, 0).x;

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i, j) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // apply PCF
    const int numSamples = 16;
    for (int i = -numSamples/2; i <= numSamples/2; ++i)
    {
        for (int j = -numSamples/2; j <= numSamples/2; ++j)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i, j) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= float(numSamples * numSamples);

    shadow = clamp(shadow + bias, 0.0, 1.0);
    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

bool isOnPath(vec3 fragPos, vec3 pathedPoint, vec3 nextPathedPoint) {
    float distToSegment = distance(nextPathedPoint, pathedPoint);
    float distToStart = distance(fragPos, pathedPoint);
    float distToEnd = distance(fragPos, nextPathedPoint);

    float dotProduct = dot(normalize(nextPathedPoint - pathedPoint), normalize(fragPos - pathedPoint));

    return (dotProduct >= 0.0 && dotProduct <= distToSegment && distToStart <= distToSegment && distToEnd <= distToSegment);
}

const float PI = 3.14159265359;

vec3 calculateAmbient(vec3 color) {
    return 0.3 * color * sun.ambient;
}

vec3 calculateDiffuse(vec3 normal, vec3 color) {
    float diff = max(dot(normal, sun.direction), 0.0);
    return diff * color * sun.diffuse;
}

vec3 calculateSpecular(vec3 normal, vec3 viewPos, vec3 fragPos) {
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(sun.direction + viewDir);

    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    return spec * sun.specular;
}


vec3 calculateLighting(vec3 fragPos, vec3 normal, vec3 viewPos, vec3 color, float shadow) {
    vec3 ambient = calculateAmbient(color);
    vec3 diffuse = calculateDiffuse(normal, color);

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(sun.direction + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.2);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    float noiseVal = fbm(fs_in.TexCoords * 10.0);
    lighting += vec3(noiseVal * 0.1);

    float sunHeight = sun.position.y;
    float sunHeightFactor = clamp(sunHeight, 0.3, 1.0);
    lighting = mix(lighting, lighting * 0.5, sunHeightFactor);

    if (pathedPointsCount > 0) {
        for (int i = 0; i < pathedPointsCount; i++) {
            vec3 pathedPoint = pathedPoints[i];
            vec3 nextPathedPoint = pathedPoints[(i + 1) % pathedPointsCount];

            if (isOnPath(fragPos, pathedPoint, nextPathedPoint)) {
                lighting = pathColor;
                break;
            }
        }
    }

    return lighting;
}


void main() {
    vec3 color = grassColor;

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    vec3 lighting = calculateLighting(fs_in.FragPos, fs_in.Normal, viewPos, color, shadow);

    FragColor = vec4(lighting, 1.0);
}
