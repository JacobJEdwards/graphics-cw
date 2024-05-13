//
// Created by Jacob Edwards on 27/02/2024.
//

#include "Sun.h"

#include "Config.h"
#include <GL/glew.h>
#include <cmath>
#include <glm/trigonometric.hpp>
#include <glm/common.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <graphics/Color.h>

#include "utils/ShaderManager.h"
#include "imgui/imgui.h"
#include "renderables/Entity.h"

Sun::Sun() : Entity("../Assets/objects/sun/sun.obj") {
    shader = ShaderManager::GetInstance().get("Sun");
    moon = Entity("../Assets/objects/moon/moon.obj");

    auto transform = Config::IDENTITY_MATRIX;
    transform = glm::scale(transform, glm::vec3(scale));

    Entity::transform(transform);
    transform = glm::scale(transform, glm::vec3(10.0F));
    moon.transform(transform);

    attributes.position.x = 0.0F;
    attributes.position.y = 0.0F;
    attributes.position.z = 0.0F;
    attributes.gravityAffected = false;

    moon.attributes.position.x = 0.0F;
    moon.attributes.position.y = 0.0F;
    moon.attributes.position.z = 50.0F;
    moon.attributes.gravityAffected = false;

    moon.setShader(ShaderManager::GetInstance().get("Moon"));
}

void Sun::update(const float deltaTime) {
    if (!move) {
        return;
    }

    float localSpeed;
    if (attributes.position.y < 10.0F) {
        localSpeed = 20.0F;
    } else {
        localSpeed = 1.0F;
    }

    attributes.update(deltaTime * speed * localSpeed);
    moon.attributes.update(deltaTime * speed * localSpeed);

    angle += 0.05F * deltaTime * speed * localSpeed;
    constexpr float orbitRadius = 1000.0F;
    constexpr float height = 20.0F;
    const float x = orbitRadius * std::cos(angle);
    const float y = orbitRadius * std::sin(angle);
    const float z = height * std::sin(0.5F * angle);


    attributes.position.x = x;
    attributes.position.y = y;
    attributes.position.z = z;

    // moon orbit
    moon.attributes.position.x = -x;
    moon.attributes.position.y = -y;
    moon.attributes.position.z = -z;

    const float currentHeight = attributes.position.y;
    constexpr float max = 10.0F;
    constexpr float min = -10.0F;

    const float t = (currentHeight - min) / (max - min);

    const float angleRad = glm::radians(angle);

    const float tHorizontal = std::abs(glm::cos(angleRad));

    float mixFactor = glm::clamp(t, 0.0F, 1.0F);
    mixFactor *= glm::clamp(tHorizontal, 0.0F, 1.0F);
    colorMixFactor = mixFactor;

    // darkness factor -> if sun is below horizon, increase darkness
    darknessFactor = glm::clamp(1.0F - t, 0.0F, 0.8F);


    moon.attributes.transform = glm::translate(Config::IDENTITY_MATRIX, moon.attributes.position);
    moon.attributes.transform = glm::scale(moon.attributes.transform, glm::vec3(2.5F));
}

void Sun::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    GLenum prevDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&prevDepthFunc));

    glDepthFunc(GL_LEQUAL);

    const auto newView = glm::mat4(glm::mat3(view));

    Entity::draw(newView, projection);
    moon.draw(newView, projection);

    glDepthFunc(prevDepthFunc);
}

void Sun::setPosition(const glm::vec3 &pos) {
    attributes.position = pos;
    moon.attributes.position = -pos;
}

[[nodiscard]] auto Sun::getDirection() const -> glm::vec3 { return glm::normalize(attributes.position); }

void Sun::setScale(const float newScale) { scale = newScale; }

[[nodiscard]] auto Sun::getPosition() const -> glm::vec3 { return attributes.position; }


auto Sun::getDiffuse() const -> glm::vec3 {
    const auto color = glm::mix(Color::WHITE, Color::ORANGE, colorMixFactor * 0.9F);

    return glm::mix(color, Color::DARKGREY, darknessFactor);
}

auto Sun::getSpecular() const -> glm::vec3 {
    const auto color = glm::mix(Color::WHITE, Color::ORANGE, colorMixFactor * 0.7F);

    return glm::mix(color, Color::DARKGREY, darknessFactor);
}

auto Sun::getAmbient() const -> glm::vec3 {
    const auto color = glm::mix(Color::WHITE, Color::ORANGE, colorMixFactor * 0.5F);

    return glm::mix(color, Color::DARKGREY, darknessFactor);
}


void Sun::interface() {
    ImGui::Begin("Sun");
    ImGui::Checkbox("Update Position", &move);
    ImGui::SliderFloat("Speed", &speed, 0.0F, 20.0F);
    ImGui::End();
}
