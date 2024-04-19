#version 330 core
out vec4 FragColor;


struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in vec3 Tangent;
in vec3 Bitangent;
in vec4 FragPosLightSpace;

uniform sampler2D shadowMap;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

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

vec3 calculateBlinnPhongLighting(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, vec3 lightColor, vec3 surfaceColor, float shininess) {
    // Normalize vectors
    lightDirection = normalize(lightDirection);
    viewDirection = normalize(viewDirection);
    surfaceNormal = normalize(surfaceNormal);

    vec3 halfwayDir = normalize(lightDirection + viewDirection);

    float diffuseFactor = max(dot(lightDirection, surfaceNormal), 0.0);

    float specularFactor = 0.0;

    if (diffuseFactor > 0.0) {
        specularFactor = pow(max(dot(surfaceNormal, halfwayDir), 0.0), shininess);
    }

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diffuseFactor * lightColor * surfaceColor;
    vec3 specular = specularFactor * lightColor;

    return ambient + diffuse + specular;
}


void main() {
    // float shadow = ShadowCalculation(FragPosLightSpace);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec4 texColor = texture(material.texture_diffuse1, TexCoords);

    vec3 result = calculateBlinnPhongLighting(lightDir, viewDir, norm, light.diffuse * 2.0, texColor.xyz, material.shininess);

    result *= texColor.a;

    // Apply shadow
    // result *= shadow;

    // light is sun, descrease if below horizon
    float sunHeight = light.position.y;
    float sunFactor = clamp(sunHeight, 0.3, 1.0);
    result *= sunFactor;

    FragColor = vec4(result, 1.0);
}
