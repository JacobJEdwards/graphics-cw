#version 410 core

in VS_OUT {
    vec2 TexCoords;
} fs_in;

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light sun;

uniform sampler2D screenTexture;
uniform sampler2D depthTexture;

uniform vec3 viewPos;
uniform vec3 viewDir;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float gamma = 2.2;
uniform float exposure = 1.0;
uniform float contrast = 1.2;
uniform float saturation = 1.5;
uniform float brightness = 1.0;
uniform float bloomThreshold = 0.98;
uniform float bloomIntensity = 1.4;
uniform float vignetteStrength = 0.3;
uniform float blurAmount = 0.008;
uniform bool blur = false;
uniform float blurTime = 0.0;

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

vec3 applyGodrays(vec3 color, vec2 texCoords) {
    vec3 sunDir = normalize(sun.position - vec3(texCoords, 0.0));
    float sunDot = dot(sunDir, sun.direction);
    float sunFactor = smoothstep(0.0, 0.5, sunDot);

    return color + sunFactor * sun.diffuse;
}


void main() {
    vec4 color = texture(screenTexture, fs_in.TexCoords);
    float depthValue = texture(depthTexture, fs_in.TexCoords).r;

    // fog based on depth
    float fogFactor = smoothstep(10.0, 100.0, depthValue);
    color.rgb = mix(color.rgb, vec3(0.8, 0.8, 0.8), fogFactor);


    color.rgb = applyGammaCorrection(color.rgb);
    color.rgb = applyExposure(color.rgb);
    color.rgb = applyContrast(color.rgb);
    color.rgb = applySaturation(color.rgb);
    color.rgb = applyBrightness(color.rgb);
    color.rgb = applyBloom(color.rgb);
    // color.rgb = applyGodrays(color.rgb, fs_in.TexCoords);

    float sunsetFactor = max(dot(normalize(sun.position), vec3(0, -1, 0)), 0.0);
    color.rgb = mix(color.rgb, color.rgb * sun.diffuse, sunsetFactor);

    float vignette = calculateVignette(fs_in.TexCoords);
    color.rgb *= vignette;

    if (blur) {
        vec4 blurredColorX = applyBlur(screenTexture, fs_in.TexCoords, vec2(1.0, 0.0), blurAmount);
        vec4 blurredColorY = applyBlur(screenTexture, fs_in.TexCoords, vec2(0.0, 1.0), blurAmount);
        color = mix(color, (blurredColorX + blurredColorY), 0.8);
    }

    FragColor = color;
}
