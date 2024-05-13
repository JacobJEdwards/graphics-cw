#ifndef _lighting_h_
#define _lighting_h_

uniform sampler2D shadowMap;

#include "lights.glsl"

/*
* https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
*/


float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    vec3 normal = normalize(fs_in.Normal);
    float bias = max(0.05 * (1.0 - dot(normal, lights.sun.direction)), 0.005);

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

/*
* https://learnopengl.com/lighting/basic-lighting
*/
vec3 calculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, vec3 color, float shininess, float shadow) {
    vec3 halfwayDir = normalize(light.direction + viewDir);

    float diffuseFactor = max(dot(light.direction, normal), 0.0);

    float specularFactor = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    specularFactor *= diffuseFactor;

    vec3 ambient = 0.1 * light.ambient;
    vec3 diffuse = diffuseFactor * color;
    vec3 specular = specularFactor * light.specular * light.diffuse;

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

    lighting = clamp(lighting, 0.0, 1.0);

    return lighting;
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color, float shininess, float shadow) {
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 ambient = light.ambient * color;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * color;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = spec * light.specular * color;

    diffuse *= attenuation;
    specular *= attenuation;
    ambient *= attenuation;

    vec3 lighting = (ambient + (diffuse + specular));

    lighting = clamp(lighting, 0.0, 1.0);

    return lighting;
}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 color, float shininess, float shadow) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * color;
    vec3 diffuse = diff * light.diffuse * color;
    vec3 specular = spec * light.specular * color;

    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    ambient *= attenuation * intensity;

    return (ambient + (diffuse + specular));

}

vec3 calculateLighting(vec3 fragPos, vec3 normal, vec3 viewPos, vec3 color, float shininess) {
    vec3 viewDir = normalize(viewPos - fragPos);
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    normal = normalize(normal);

    vec3 lighting = calculateDirectionalLight(lights.sun, normal, viewDir, color, shininess, shadow);

    for (int i = 0; i < lights.pointLightsCount; i++) {
        lighting += calculatePointLight(lights.pointLights[i], normal, fragPos, viewDir, color, shininess, shadow);
    }

    // lighting += calculateSpotLight(lights.spotLight, normal, fragPos, viewDir, color, shininess, shadow);


    return lighting;
}

vec3 calculateLightingBasic(vec3 fragPos, vec3 normal, vec3 viewPos, vec3 color, float shininess) {
    // no shadow
    vec3 viewDir = normalize(viewPos - fragPos);
    normal = normalize(normal);

    vec3 lighting = calculateDirectionalLight(lights.sun, normal, viewDir, color, shininess, 0.0);

    for (int i = 0; i < lights.pointLightsCount; i++) {
        lighting += calculatePointLight(lights.pointLights[i], normal, fragPos, viewDir, color, shininess, 0.0);
    }

    // lighting += calculateSpotLight(lights.spotLight, normal, fragPos, viewDir, color, shininess, 0.0);

    return lighting;
}

vec3 calculateTerrainLighting(vec3 fragPos, vec3 normal, vec3 viewPos, vec3 color, float shadow) {
    vec3 ambient = 0.3 * color;
    float diff = max(dot(normal, lights.sun.direction), 0.0);
    vec3 diffuse = diff * color * lights.sun.diffuse;

    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lights.sun.direction + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
    vec3 specular = spec * lights.sun.specular;

    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    float lightFactor = max(dot(lights.sun.direction, vec3(0.0, 1.0, 0.0)), 0.1);
    lighting *= lightFactor;

    for (int i = 0; i < lights.pointLightsCount; i++) {
        lighting += calculatePointLight(lights.pointLights[i], normal, fragPos, viewDir, color, 32.0, shadow);
    }

    return lighting;
}

#endif