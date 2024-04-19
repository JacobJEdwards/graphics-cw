#version 410 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in float height;
in vec4 FragPosLightSpace;

uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;


// points that have been pathed
uniform vec3 pathedPoints[256];
uniform int pathedPointsCount = 0;
uniform float pathedPointsRadius = 5.0;
uniform float pathDarkness = 0.5;


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

    float shadow = 0.0;
    float bias = 0.005;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i, j) * 1.0/1024.0).r;
            shadow += projCoords.z - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    shadow = clamp(shadow + bias, 0.0, 1.0);

    return shadow;
}

void main() {
    vec3 color = vec3(0.2, 0.8, 0.2);
    float shadow = ShadowCalculation(FragPosLightSpace);

    for (int i = 0; i < pathedPointsCount; i++) {
        vec3 pathedPoint = pathedPoints[i];
        float dist = distance(pathedPoint, FragPos);
        if (dist < pathedPointsRadius) {
            color = mix(color, vec3(0.2, 0.1, 0.0), pathDarkness);
        }
    }


    vec3 ambient = 0.3 * color;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.2);

    // apply shadow
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    // add random noise to the grass
    float noiseVal = fbm(TexCoords * 10.0);
    lighting += vec3(noiseVal * 0.1);

    FragColor = vec4(lighting, 1.0);
}
