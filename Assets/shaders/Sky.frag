#version 410 core

in vec3 FragPos;
in vec3 FragPosWorld;

out vec4 FragColor;

uniform vec3 sunColor = vec3(1.0, 0.9, 0.6);
uniform vec3 sunPos = vec3(0.0, 100.0, 0.0);// Position of the sun

uniform vec3 horizonColor = vec3(0.5, 0.6, 0.7);// Color of the horizon
uniform vec3 zenithColor = vec3(0.4, 0.7, 1.0);// Color of the zenith (top of the sky)
uniform vec3 cloudColor = vec3(1.0, 1.0, 1.0);// Color of clouds

uniform float cloudCover = 0.6;// Amount of cloud cover
uniform float cloudSharpness = 0.5;// Sharpness of the clouds
uniform float cloudDensity = 0.6;// Density of the clouds
uniform float cloudScale = 0.02;// Scale of the clouds
uniform float cloudSpeed = 0.01;// Speed of cloud movement

uniform float sunSize = 8.0;// Size of the sun
uniform float sunSharpness = 50.0;// Sharpness of the sun
uniform float sunIntensity =  1.5;// Intensity of sunlight

uniform float horizonHeight = 0.4;// Height of the horizon
uniform float horizonSharpness = 0.3;// Sharpness of the horizon

uniform float zenithHeight = 0.3;// Height of the zenith
uniform float zenithSharpness = 0.3;// Sharpness of the zenith

uniform vec3 skyColor = vec3(0.5, 0.6, 0.7);

uniform float time = 0.0;// Time variable for animation or time-based effects

// stars
uniform float starDensity = 0.00001;// Density of stars
uniform float starSharpness = 1.0;// Sharpness of stars
uniform float starSize = 0.01;// Size of stars

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
    vec3 cloud = cloudColor * cloudAmount * cloudNoise * cloudDensityFactor;

    // darkening the sky at the zenith
    float zenithFactor = pow(max(0.0, FragPos.y), zenithSharpness);
    finalSkyColor = mix(finalSkyColor, zenithColor, zenithFactor);

    finalSkyColor = mix(finalSkyColor, vec3(0.001, 0.001, 0.001), 1.0 - sunHeightFactor);
    cloud = mix(cloud, vec3(0.0, 0.0, 0.0), 1.0 - sunHeightFactor);

    // stars
    float starProb = step(hash(FragPosWorld.x * 0.1 + FragPosWorld.y * 0.1), starDensity);
    vec3 stars = vec3(1.0, 1.0, 1.0) * starProb;

    stars = mix(vec3(0.0, 0.0, 0.0), stars, 1.0 - sunHeightFactor);

    finalSkyColor += sun;

    FragColor = vec4(finalSkyColor + cloud + stars, 1.0);
}