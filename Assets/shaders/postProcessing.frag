#version 410 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec3 sunPosition;
uniform vec3 viewPos;

uniform float gamma = 2.2;
uniform float exposure = 1.0;
uniform float contrast = 1.2;// Adjusted contrast for better visibility
uniform float saturation = 1.5;// Increased saturation for more vibrant colors
uniform float brightness = 1.0;
uniform float bloomThreshold = 0.98;
uniform float bloomIntensity = 1.4;
uniform float vignetteStrength = 0.3;
uniform float blurAmount = 0.008;
uniform bool blur = false;
uniform float blurTime = 0.0;

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

vec4 applyBlur(sampler2D tex, vec2 texCoords, vec2 direction, float blurAmount) {
    vec4 color = vec4(0.0);
    vec2 step = direction * blurAmount * blurTime;

    for (int i = -5; i <= 5; ++i) {
        color += texture(tex, texCoords + float(i) * step);
    }

    return color / 11.0;
}

void main() {
    vec4 color = texture(screenTexture, TexCoords);

    color.rgb = applyGammaCorrection(color.rgb);
    color.rgb = applyExposure(color.rgb);
    color.rgb = applyContrast(color.rgb);
    color.rgb = applySaturation(color.rgb);
    color.rgb = applyBrightness(color.rgb);
    color.rgb = applyBloom(color.rgb);

    float sunsetFactor = max(dot(normalize(sunPosition), vec3(0, -1, 0)), 0.0);
    color.rgb = mix(color.rgb, color.rgb * orangeTint, sunsetFactor);

    float vignette = calculateVignette(TexCoords);
    color.rgb *= vignette;

    if (blur) {
        vec4 blurredColorX = applyBlur(screenTexture, TexCoords, vec2(1.0, 0.0), blurAmount);
        vec4 blurredColorY = applyBlur(screenTexture, TexCoords, vec2(0.0, 1.0), blurAmount);
        color = mix(color, (blurredColorX + blurredColorY), 0.8);

    }

    FragColor = color;
}
