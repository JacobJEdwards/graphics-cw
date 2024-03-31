#version 410 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec3 sunPosition;
uniform vec3 viewPos;

const float gamma = 2.2;
const float exposure = 1.0;
const float contrast = 1.2;// Adjusted contrast for better visibility
const float saturation = 1.5;// Increased saturation for more vibrant colors
const float brightness = 1.0;

const float bloomThreshold = 0.7;
const float bloomIntensity = 1.5;
const float vignetteStrength = 0.3;

const vec3 orangeTint = vec3(1.0, 0.5, 0.2);

vec3 applyGammaCorrection(vec3 color) {
    return pow(color, vec3(1.0 / gamma));
}

vec3 applyExposure(vec3 color) {
    return color * exposure;
}

vec3 applyContrast(vec3 color) {
    return mix(vec3(0.5), color, contrast);
}

vec3 applySaturation(vec3 color) {
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    return mix(vec3(luminance), color, saturation);
}

vec3 applyBrightness(vec3 color) {
    return color * brightness;
}

vec3 applyBloom(vec3 color) {
    vec3 bloom = max(color - bloomThreshold, 0.0);
    bloom = pow(bloom, vec3(1.0 / gamma));
    return color + bloom * bloomIntensity;
}

float calculateVignette(vec2 texCoords) {
    return smoothstep(1.0, 1.0 - vignetteStrength, length(texCoords - 0.5));
}

void main() {
    vec4 color = texture(screenTexture, TexCoords);

    // color.rgb = applyGammaCorrection(color.rgb);
    color.rgb = applyExposure(color.rgb);
    color.rgb = applyContrast(color.rgb);
    color.rgb = applySaturation(color.rgb);
    color.rgb = applyBrightness(color.rgb);
    // color.rgb = applyBloom(color.rgb);

    float sunsetFactor = max(dot(normalize(sunPosition), vec3(0, -1, 0)), 0.0);
    color.rgb = mix(color.rgb, color.rgb * orangeTint, sunsetFactor);

    float vignette = calculateVignette(TexCoords);
    color.rgb *= vignette;

    FragColor = color;
}
