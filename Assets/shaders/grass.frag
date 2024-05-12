#version 410 core


out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

#include "lights.glsl"
#include "camera.glsl"

const vec3 grassColor = vec3(0.2, 0.8, 0.2);

void main() {
    vec3 color = grassColor;

    vec3 ambient = 0.3 * color;

    // diffuse
    vec3 norm = normalize(fs_in.Normal);
    float diff = max(dot(norm, lights.sun.direction), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(camera.position - fs_in.FragPos);
    vec3 halfwayDir = normalize(lights.sun.direction + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.2) * (0.2);

    vec3 lighting = (ambient + diffuse + specular) * color;

    // if sun is below the horizon, make the grass darker, smooth

    FragColor = vec4(lighting, 1.0);
}
