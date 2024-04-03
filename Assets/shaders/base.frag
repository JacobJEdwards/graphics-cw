#version 410 core

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in float height;

uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform mat4 lightSpaceMatrix;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main() {
    vec3 color;
    // first calculate the shadow
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);

    if (height < -0.1) {
        color = vec3(0.0, 0.0, 1.0);
    } else if (height < 0.3) {
        color = vec3(0.0, 1.0, 0.0);
    } else if (height < 0.6) {
        color = vec3(0.5, 0.5, 0.5);
    } else {
        color = vec3(1.0, 1.0, 1.0);
    }

    // ambient
    vec3 ambient = 0.3 * color;
    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.2);

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    FragColor = vec4(lighting, 1.0);
}