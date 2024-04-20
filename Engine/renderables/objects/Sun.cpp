//
// Created by Jacob Edwards on 27/02/2024.
//

#include "Sun.h"

#include "Config.h"
#include <GL/glew.h>
#include <cmath>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>

#include "utils/ShaderManager.h"
#include "imgui/imgui.h"
#include "renderables/Entity.h"

Sun::Sun() : Entity("../Assets/objects/sun/sun.obj") {
    shader = ShaderManager::GetInstance().get("Sun");
    moon = Entity("../Assets/objects/moon/moon.obj");

    auto transform = Config::IDENTITY_MATRIX;
    transform = glm::scale(transform, glm::vec3(scale));

    Entity::transform(transform);
    moon.transform(transform);

    attributes.position.x = 0.0F;
    attributes.position.y = 0.0F;
    attributes.position.z = -50.0F;

    moon.attributes.position.x = 0.0F;
    moon.attributes.position.y = 0.0F;
    moon.attributes.position.z = 50.0F;

    moon.setShader(ShaderManager::GetInstance().get("Base"));
}

void Sun::update(const float deltaTime) {
    if (!move) {
        return;
    }

    attributes.update(deltaTime * speed);

    angle += 0.05F * deltaTime * speed;
    constexpr float orbitRadius = 100.0F;
    constexpr float height = 20.0F;
    const float x = orbitRadius * std::cos(angle);
    const float y = orbitRadius * std::sin(angle);
    const float z = height * std::sin(0.5 * angle);


    attributes.position.x = x;
    attributes.position.y = y;
    attributes.position.z = z;

    moon.attributes.position.x = -x;
    moon.attributes.position.y = -y;
    moon.attributes.position.z = -z;
}

void Sun::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    GLenum prevDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&prevDepthFunc));

    glDepthFunc(GL_LEQUAL);

    const auto newView = glm::mat4(glm::mat3(view));

    Entity::draw(newView, projection);


    moon.draw(view, projection);

    glDepthFunc(prevDepthFunc);
}

void Sun::setPosition(const glm::vec3 &pos) {
    attributes.position = pos;
    moon.attributes.position = -pos;
}

[[nodiscard]] auto Sun::getDirection() const -> glm::vec3 { return glm::normalize(attributes.position); }

void Sun::setScale(const float newScale) { scale = newScale; }

[[nodiscard]] auto Sun::getPosition() const -> glm::vec3 { return attributes.position; }

void Sun::interface() {
    ImGui::Begin("Sun");
    ImGui::Checkbox("Update Position", &move);
    ImGui::SliderFloat("Speed", &speed, 0.0F, 20.0F);
    ImGui::End();
}
