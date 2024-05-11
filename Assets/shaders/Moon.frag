#version 410 core
out vec4 FragColor;
in vec2 TexCoords;

struct Material {
    vec4 ambient;
    vec4 specular;
    vec4 emissive;
    vec4 diffuse;
    float shininess;
};

struct Light {
    vec4 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light sun;
uniform vec3 viewPos;
uniform vec3 viewDir;

// moon shader

void main() {
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
