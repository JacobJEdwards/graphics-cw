#version 410 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture1;

const float glowIntensity = 0.8;

void main() {
    vec4 texColor = texture(texture1, TexCoords);

    texColor.rgb *= 2.0;

    vec4 glowColor = vec4(1.0, 0.8, 0.2, 1.0);
    vec4 glow = glowIntensity * glowColor * smoothstep(0.2, 0.0, length(TexCoords - 0.5));

    texColor += glow;

    FragColor = texColor;
}
