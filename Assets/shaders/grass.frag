#version 410 core

struct Light {
    vec3 position;
    vec3 color;
};

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
} fs_in;

uniform Light light;
uniform vec3 viewPos;

const vec3 grassColor = vec3(0.2, 0.8, 0.2);

void main() {
    vec3 color = grassColor;

    vec3 ambient = 0.3 * color;

    // diffuse
    vec3 norm = normalize(fs_in.Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.2);

    // apply shadow
    vec3 lighting = (ambient * (diffuse + specular)) * color;

    // if sun is below the horizon, make the grass darker, smooth
    float sunHeight = light.position.y;
    float sunHeightFactor = clamp(sunHeight / 10.0, 0.0, 1.0);
    lighting = mix(lighting, lighting * 0.5, sunHeightFactor);

    FragColor = vec4(color * 0.3, 1.0);
}
