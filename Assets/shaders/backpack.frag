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


in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = calculateBlinnPhongLighting(lightDir, viewDir, norm, light.diffuse * 2.0, vec3(texture(material.texture_diffuse1, TexCoords)), material.shininess);

    if (light.position.y < -0.2) {
        result = result * 0.1;
    }

    FragColor = vec4(result, 1.0);
}