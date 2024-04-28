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
} fs_in;

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

out vec4 FragColor;

// bling phong lighting
vec3 BlingPhongLighting(vec3 normal, vec3 viewDir, vec3 lightDir, vec3 lightColor, vec3 ambient, vec3 diffuse, vec3 specular, float shininess) {
    vec3 ambientColor = ambient * lightColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseColor = diffuse * diff * lightColor;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specularColor = specular * spec * lightColor;

    return (ambientColor + diffuseColor + specularColor);
}

void main() {
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);

    vec3 result = BlingPhongLighting(normal, viewDir, lightDir, light.diffuse, material.ambient.rgb, material.diffuse.rgb, material.specular.rgb, material.shininess);

    FragColor = material.diffuse;
}
