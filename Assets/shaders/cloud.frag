#version 410 core

struct Material {
    vec4 ambient;
    vec4 specular;
    vec4 emmisive;
    vec4 diffuse;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


in VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    vec3 Tangent;
    vec3 Bitangent;
} fs_in;

uniform Material material;
uniform Light sun;
uniform vec3 viewPos;

out vec4 FragColor;

vec3 calculateBlinnPhongLighting(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, vec3 lightColor, vec3
surfaceColor, float shininess) {
    // Normalize vectors
    lightDirection = normalize(lightDirection);
    viewDirection = normalize(viewDirection);
    surfaceNormal = normalize(surfaceNormal);

    vec3 halfwayDir = normalize(lightDirection + viewDirection);

    float diffuseFactor = max(dot(lightDirection, surfaceNormal), 0.0);


    float specularFactor = pow(max(dot(surfaceNormal, halfwayDir), 0.0), shininess);

    specularFactor *= diffuseFactor;

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = diffuseFactor * lightColor * surfaceColor;
    vec3 specular = specularFactor * lightColor;

    // shadow
    vec3 lighting = (ambient + diffuse + specular);

    return lighting;
}

void main() {
    vec3 baseColor = vec3(1.0, 1.0, 1.0);
    vec3 finalColor = baseColor;

    /*
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    vec3 lighting = calculateBlinnPhongLighting(sun.direction, viewDir, normal, sun.ambient, baseColor, material.shininess);

    vec3 finalColor = lighting * 10.0;      // Scale output color
    */

    // Apply sun factor to the final color
    float sunHeight = sun.position.y;
    float sunFactor = clamp(sunHeight, 0.2, 1.0);
    finalColor *= sunFactor;

    // Output final color
    FragColor = vec4(finalColor, 1.0);

    // Adjust alpha
    FragColor.a /= 1.5;

    // Discard fragments with low alpha
    if (FragColor.a < 0.2) {
        discard;
    }
}
