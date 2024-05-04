#version 410 core

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material {
    vec4 ambient;
    vec4 specular;
    vec4 emmisive;
    vec4 diffuse;
    float shininess;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
    vec4 FragPosLightSpace;
} fs_in;

uniform Material material;
uniform Light sun;
uniform vec3 viewPos;
uniform vec4 color;

uniform sampler2D shadowMap;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(sun.position - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    float texelSize = 1.0 / textureSize(shadowMap, 0).x;

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i, j) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // Apply PCF
    const int numSamples = 16;
    for (int i = -numSamples/2; i <= numSamples/2; ++i)
    {
        for (int j = -numSamples/2; j <= numSamples/2; ++j)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(i, j) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= float(numSamples * numSamples);

    shadow = clamp(shadow + bias, 0.0, 1.0);
    if (projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

vec3 calculateBlinnPhongLighting(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, vec3 lightColor, vec3
surfaceColor, float shininess) {
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
    vec3 diffuse = diffuseFactor * lightColor * surfaceColor * color.rgb;
    vec3 specular = specularFactor * lightColor;

    // shadow calculation
    // float shadow = ShadowCalculation(fs_in.FragPosLightSpace);

    //return (ambient + (1.0 - shadow) * (diffuse + specular));

    return (ambient + diffuse + specular);

}

void main() {
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(sun.position - fs_in.FragPos);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 result = calculateBlinnPhongLighting(lightDir, viewDir, normal, vec3(material.diffuse), vec3(material.specular), material.shininess);

    FragColor = vec4(result, 1.0);
}



