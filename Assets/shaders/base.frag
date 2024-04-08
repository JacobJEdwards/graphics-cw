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

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Soft shadows with PCF
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

    // Apply additional bias to avoid self-shadowing artifacts
    shadow = clamp(shadow + bias, 0.0, 1.0);

    return shadow;
}

void main() {
    vec3 color;
    float shadow = ShadowCalculation(FragPosLightSpace);

    // Terrain material based on height
    if (height < -0.1) {
        color = vec3(0.0, 0.0, 1.0);
    } else if (height < 0.3) {
        color = vec3(0.0, 1.0, 0.0);
    } else if (height < 0.6) {
        color = vec3(0.5, 0.5, 0.5);
    } else {
        color = vec3(1.0, 1.0, 1.0);
    }

    // Ambient light
    vec3 ambient = 0.3 * color;

    // Diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // Specular light
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.2);

    // Apply shadow
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    FragColor = vec4(lighting, 1.0);
}
