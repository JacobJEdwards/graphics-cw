//
// Created by Jacob Edwards on 12/05/2024.
//
/*
 *https://learnopengl.com/Lighting/Multiple-lights
 */

#ifndef LIGHTS_H
#define LIGHTS_H

#include <glm/glm.hpp>

constexpr int MAX_POINT_LIGHTS = 12;

struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

struct Lights {
    DirectionalLight sun;
    PointLight pointLight[MAX_POINT_LIGHTS];
    int pointLightCount;
    SpotLight spotLight;
};


#endif //LIGHTS_H

