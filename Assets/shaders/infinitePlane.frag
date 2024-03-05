#version 330 core
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

    // Halfway vector calculation
    vec3 halfwayDir = normalize(lightDirection + viewDirection);

    // Diffuse component
    float diffuseFactor = max(dot(lightDirection, surfaceNormal), 0.0);

    // Specular component
    float specularFactor = 0.0;
    if (diffuseFactor > 0.0) {
        specularFactor = pow(max(dot(surfaceNormal, halfwayDir), 0.0), shininess);
    }

    // Final lighting calculation
    vec3 ambient = 0.1 * lightColor; // Ambient term
    vec3 diffuse = diffuseFactor * lightColor * surfaceColor; // Diffuse term
    vec3 specular = specularFactor * lightColor; // Specular term

    return ambient + diffuse + specular;
}


in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;
in vec2 TexCoord;

uniform Light light;
uniform vec3 viewPos;

void main() {
    int x = int(TexCoord.x * 5000);
    int y = int(TexCoord.y * 5000);
    int color = (x + y) % 2;

    // lighting
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = calculateBlinnPhongLighting(lightDir, viewDir, Normal, light.diffuse, vec3(color), 128.0);
    // FragColor = vec4(color, 1.0);

    FragColor = vec4(vec3(color), 1.0);
}