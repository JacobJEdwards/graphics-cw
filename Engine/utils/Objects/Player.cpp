//
// Created by Jacob Edwards on 13/03/2024.
//

#include "Player.h"

#include "imgui/imgui.h"
#include "utils/BoundingBox.h"
#include <glm/glm.hpp>

#include "utils/Camera.h"
#include "Entity.h"

Player::Player() : Entity("../Assets/objects/person/person.obj") {
};


// possibly instead set up always as 0,1,0 ?
void Player::processKeyboard(const Direction direction, const float deltaTime) {
    if (camera->getMode() == Camera::Mode::ORBIT || camera->getMode() == Camera::Mode::PATH) {
        return;
    }

    auto front = camera->getFront();
    auto right = camera->getRight();
    auto up = camera->getUp();

    switch (direction) {
        case Direction::FORWARD:
            attributes.applyForce(front * 10.0F);
            break;
        case Direction::BACKWARD:
            attributes.applyForce(-front * 10.0F);
            break;
        case Direction::LEFT:
            attributes.applyForce(-right * 10.0F);
            break;
        case Direction::RIGHT:
            attributes.applyForce(right * 10.0F);
            break;
        case Direction::NONE:
            attributes.applyDrag(0.5F);
            break;
        case Direction::UP:
            if (mode == Mode::FPS) {
                jump();
            } else {
                attributes.applyForce(glm::vec3(0.0F, 10.0F, 0.0F));
            }
            break;
        case Direction::DOWN:
            attributes.applyForce(glm::vec3(0.0F, -10.0F, 0.0F));
            break;
    }
}

[[nodiscard]] auto Player::getCamera() const -> Camera & {
    return *camera;
}

[[nodiscard]] auto Player::getPosition() const -> glm::vec3 {
    return attributes.position;
}

void Player::update(float dt) {

    Entity::update(dt);

    camera->setPosition(attributes.position + glm::vec3(0.0F, 4.0F, 0.0F));

    if (camera->getMode() == Camera::Mode::PATH) {
        // instead update the camera view look at based on transform
        auto front = glm::normalize(glm::vec3(attributes.transform[2]));
        auto right = glm::normalize(glm::vec3(attributes.transform[0]));
        auto up = glm::normalize(glm::vec3(attributes.transform[1]));

        auto yaw = glm::degrees(std::atan2(front.z, front.x) + glm::pi<float>()) * 1.2F;
        auto pitch = glm::degrees(std::asin(front.y));

        camera->setYaw(yaw);
        camera->setPitch(pitch);

        return;
    }

    const glm::vec3 front = camera->getFront();
    const glm::vec3 modelForward = glm::normalize(glm::vec3(attributes.transform[2]));

    auto rotationRequired = glm::vec3(0.0F);

    const float dotProduct = glm::dot(modelForward, front);

    if (dotProduct < 0.99F) {
        const float angle = std::acos(dotProduct);
        const glm::vec3 axis = glm::normalize(glm::cross(modelForward, front));
        rotationRequired = axis * angle;
    }

    if (glm::length(rotationRequired) > 0.01F) {
        rotationRequired.z = 0.0F;
        rotationRequired.x = 0.0F;

        attributes.applyRotation(rotationRequired);
    }

    camera->update(dt);
}

[[nodiscard]] auto Player::shouldDraw() const -> bool {
    return drawModel;
}

void Player::shouldDraw(bool draw) {
    drawModel = draw;
}

void Player::setMode(Mode mode) {
    Player::mode = mode;

    switch (mode) {
        case Mode::FPS:
            camera->setMode(Camera::Mode::FPS);
            break;
        case Mode::ORBIT:
            camera->setMode(Camera::Mode::ORBIT);
            break;
        case Mode::FREE:
            camera->setMode(Camera::Mode::FREE);
            break;
        case Mode::FIXED:
            camera->setMode(Camera::Mode::FIXED);
            break;
        case Mode::PATH:
            camera->setMode(Camera::Mode::PATH);
            break;
    }
}

void Player::jump() {
    if (attributes.isGrounded) {
        attributes.applyForce(glm::vec3(0.0F, jumpForce, 0.0F));
    }
}

void Player::debug() const {
    ImGui::Begin("Player Debug");
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", attributes.position.x,
                attributes.position.y, attributes.position.z);
    ImGui::Text("Velocity: (%.2f, %.2f, %.2f)", attributes.velocity.x,
                attributes.velocity.y, attributes.velocity.z);
    ImGui::Text("Acceleration: (%.2f, %.2f, %.2f)",
                attributes.acceleration.x, attributes.acceleration.y,
                attributes.acceleration.z);
    ImGui::Text("Force: (%.2f, %.2f, %.2f)", attributes.force.x,
                attributes.force.y, attributes.force.z);
    ImGui::End();
}

void Player::interface() {
    ImGui::Begin("Player");
    ImGui::SliderFloat("Jump Force", &jumpForce, 0.0F, 10000.0F);
    ImGui::SliderFloat("Mass", &attributes.mass, 0.1F, 1000.0F);
    ImGui::End();
}

void Player::nitro() {
    attributes.applyForce(camera->getFront() * 200.0F);
}