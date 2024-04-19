#version 410 core

in vec3 FragPos;
in vec3 FragPosWorld;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 sunColor = vec3(1.0, 0.9, 0.6);
uniform vec3 sunPos = vec3(0.0, 100.0, 0.0);

uniform vec3 horizonColor = vec3(0.5, 0.6, 0.7);
uniform vec3 zenithColor = vec3(0.4, 0.7, 1.0);
uniform vec4 cloudColor = vec4(1.0, 1.0, 1.0, 0.95);

uniform float cloudCover = 0.6;
uniform float cloudSharpness = 0.5;
uniform float cloudDensity = 0.6;
uniform float cloudScale = 0.02;
uniform float cloudSpeed = 0.005;

uniform float sunSize = 8.0;
uniform float sunSharpness = 30.0;
uniform float sunIntensity =  1.5;

uniform float horizonHeight = 0.2;
uniform float horizonSharpness = 0.9;

uniform float zenithHeight = 0.8;
uniform float zenithSharpness = 0.9;

uniform vec3 skyColor = vec3(0.5, 0.6, 0.7);

uniform float time = 0.0;

// stars
uniform float starDensity = 0.00001;
uniform float starSharpness = 1.0;
uniform float starSize = 0.01;

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

float fbm(vec3 x) {
    float v = 0.0;
    float a = 0.5;
    float frequency = 1.0;
    float amplitude = 1.0;
    for (int i = 0; i < 5; ++i) {
        v += a * noise(x * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
        x *= 2.0;
    }
    return v;
}

void main() {
    vec3 viewDir = normalize(FragPos);
    vec3 sunDir = normalize(sunPos);
    float sunHeightFactor = clamp(sunDir.y, 0.0, 1.0);

    vec3 finalSkyColor = mix(zenithColor, horizonColor, sunHeightFactor);
    finalSkyColor += sunHeightFactor * vec3(1.0, 0.6, 0.3);

    float sunAmount = pow(max(0.0, dot(sunDir, viewDir)), sunSharpness);
    vec3 sun = sunColor * sunAmount * sunIntensity;

    float cloudAmount = pow(max(0.0, 1.0 - FragPos.y), cloudSharpness);
    float cloudNoise = fbm(FragPos * cloudScale + vec3(0.0, time * cloudSpeed, 0.0));
    float cloudDensityFactor = cloudDensity * cloudCover;
    vec4 cloud = cloudColor * cloudAmount * cloudNoise * cloudDensityFactor;

    // darkening the sky at the zenith
    float zenithFactor = pow(max(0.0, FragPos.y), zenithSharpness);
    finalSkyColor = mix(finalSkyColor, zenithColor, zenithFactor);

    finalSkyColor = mix(finalSkyColor, vec3(0.001, 0.001, 0.001), 1.0 - sunHeightFactor);
    cloud = mix(cloud, vec4(0.0, 0.0, 0.0, 0.0), 1.0 - sunHeightFactor);

    float starProb = step(hash((FragPosWorld.x + FragPosWorld.y + FragPosWorld.z) * 0.1), starDensity);
    vec3 stars = vec3(1.0, 1.0, 1.0) * starProb;

    stars = mix(vec3(0.0, 0.0, 0.0), stars, 1.0 - sunHeightFactor);

    finalSkyColor += sun;

    // make clouds slightly transparent
    finalSkyColor = mix(finalSkyColor, cloud.rgb, cloud.a);

    // add stars
    if (sunPos.y < 0.0) finalSkyColor += stars;

    FragColor = vec4(finalSkyColor, 1.0);
}