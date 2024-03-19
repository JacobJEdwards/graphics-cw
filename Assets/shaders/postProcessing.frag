#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D screenTexture;

const float exposure = 1.0; // Adjust exposure for tone mapping
const float bloomThreshold = 0.7; // Threshold for bloom effect
const float bloomIntensity = 1.5; // Intensity of bloom effect
const float vignetteStrength = 0.3; // Strength of vignette effect

void main()
{
    // Bloom effect
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
    vec3 bloomColor = max(hdrColor - bloomThreshold, 0.0) * bloomIntensity;
    hdrColor += bloomColor;

    // Tone mapping
    vec3 mappedColor = vec3(1.0) - exp(-hdrColor * exposure);

    // Vignette effect
    float distance = length(TexCoords - vec2(0.5));
    float vignette = smoothstep(0.7, 1.0, distance) * vignetteStrength;
    mappedColor *= (1.0 - vignette);

    FragColor = vec4(mappedColor, 1.0);
}
