struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

#define NR_POINT_LIGHTS 4

layout(std140) uniform Lights {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;


// padding
    vec3 padding;
/*
    PointLight pointLights[NR_POINT_LIGHTS];
    int pointLightsCount;
    SpotLight spotLight;
*/
} lights;
