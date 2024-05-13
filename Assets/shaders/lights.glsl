#ifndef _lights_h_
#define _lights_h_

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

#define NR_POINT_LIGHTS 12

struct Lights {
    DirectionalLight sun;
    PointLight pointLights[NR_POINT_LIGHTS];
    int pointLightsCount;
    SpotLight spotLight;
};

uniform Lights lights;

#endif