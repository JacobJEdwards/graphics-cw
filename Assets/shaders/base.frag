#version 410 core

struct Light {
    vec3 position;
    vec3 color;
};

out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;
uniform vec3 viewPos;
uniform Light light;

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
    float z = 2.0;
    float f = 0.0;
    f += 0.5000 * noise(p); p = p * 2.02;
    f += 0.2500 * noise(p); p = p * 2.03;
    f += 0.1250 * noise(p); p = p * 2.01;
    f += 0.0625 * noise(p);
    return f / 0.9375;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

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

    // Apply PCF
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

void main() {
    vec3 color = grassColor;
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    for (int i = 0; i < pathedPointsCount - 1; i++) {
        vec3 pathedPoint = pathedPoints[i];
        vec3 nextPathedPoint = pathedPoints[i + 1];

        float distToSegment = distance(nextPathedPoint, pathedPoint);
        float distToStart = distance(fs_in.FragPos, pathedPoint);
        float distToEnd = distance(fs_in.FragPos, nextPathedPoint);

        float dotProduct = dot(normalize(nextPathedPoint - pathedPoint), normalize(fs_in.FragPos - pathedPoint));

        if (dotProduct >= 0.0 && dotProduct <= distToSegment && distToStart <= distToSegment && distToEnd <= distToSegment) {
            color = mix(color, pathColor, pathDarkness);
            break;
        }
    }


    vec3 ambient = 0.3 * color;

    // diffuse
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.2) * (0.2);

    // apply shadow
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    // add random noise to the grass
    float noiseVal = fbm(fs_in.TexCoords * 10.0);
    lighting += vec3(noiseVal * 0.1);

    // if sun is below the horizon, make the grass darker, smooth
    float sunHeight = light.position.y;
    float sunHeightFactor = clamp(sunHeight / 10.0, 0.0, 1.0);
    lighting = mix(lighting, lighting * 0.5, sunHeightFactor);


    FragColor = vec4(lighting, 1.0);
}
