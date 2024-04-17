//
// Created by Jacob Edwards on 13/03/2024.
//

#include "Player.h"

#include "BumperCar.h"
#include "Config.h"
#include "imgui/imgui.h"
#include <cmath>
#include <glm/geometric.hpp>
#include <memory>
#include <glm/ext/matrix_transform.hpp>

#include "utils/Camera.h"
#include "Entity.h"

Player::Player(const Mode mode) : Entity("../Assets/objects/person/person.obj") {
    this->mode = mode;

    auto carModelMat = Config::IDENTITY_MATRIX;
    carModelMat = glm::translate(carModelMat, glm::vec3(0.0F, 10.0F, 0.0F));

    switch (mode) {
        case Mode::FPS:
            camera->setMode(Camera::Mode::FPS);
            break;
        case Mode::ORBIT:
            attributes.gravityAffected = false;
            drawModel = false;
            camera->setMode(Camera::Mode::ORBIT);
            break;
        case Mode::FREE:
            attributes.gravityAffected = false;
            camera->setMode(Camera::Mode::FREE);
            break;
        case Mode::FIXED:
            drawModel = false;
            attributes.gravityAffected = false;
            camera->setMode(Camera::Mode::FIXED);
            break;
        case Mode::PATH:
            camera->setMode(Camera::Mode::PATH);
            car = std::make_shared<BumperCar>();
            car->setMode(BumperCar::Mode::PATHED);
            car->transform(carModelMat);
            break;
        case Mode::DRIVE:
            camera->setMode(Camera::Mode::DRIVE);
            car = std::make_shared<BumperCar>();
            car->setMode(BumperCar::Mode::NONE);
            carModelMat = glm::scale(carModelMat, glm::vec3(4.0F));
            car->transform(carModelMat);
            car->shouldDrawPlayer(false);
            car->attributes.gravityAffected = false;
            car->attributes.position = attributes.position;
            box = car->getBoundingBox();
    }
}


void Player::processKeyboard(const Direction direction, const float /*deltaTime*/) {
    if (mode == Mode::ORBIT || mode == Mode::PATH || mode == Mode::FIXED) {
        return;
    }

    glm::vec3 front = camera->getFront();
    glm::vec3 right = camera->getRight();

    if (mode == Mode::DRIVE) {
        front = glm::normalize(glm::vec3(car->attributes.transform[2]));
        right = glm::normalize(glm::cross(front, glm::vec3(0.0F, 1.0F, 0.0F)));
    }

    switch (direction) {
        case Direction::FORWARD:
            attributes.applyForce(front * 30.0F);
            break;
        case Direction::BACKWARD:
            attributes.applyForce(-front * 10.0F);
            break;
        case Direction::LEFT:
            if (mode == Mode::DRIVE) {
                car->attributes.applyRotation(glm::vec3(0.0F, 0.1F, 0.0F));
            } else {
                attributes.applyForce(-right * 10.0F);
            }
            break;
        case Direction::RIGHT:
            if (mode == Mode::DRIVE) {
                car->attributes.applyRotation(glm::vec3(0.0F, -0.1F, 0.0F));
            } else {
                attributes.applyForce(right * 10.0F);
            }
            break;
        case Direction::NONE:
            attributes.applyDrag(0.5F);
            break;
        case Direction::UP:
            if (mode == Mode::DRIVE) {
                nitro();
            } else if (mode == Mode::FPS) {
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

void Player::update(const float dt) {
    Entity::update(dt);

    if (mode == Mode::PATH) {
        const glm::vec3 pos = car->attributes.position;
        attributes.position = pos;
        camera->setPosition(attributes.position + glm::vec3(0.0F, 4.5F, 0.0F));

        const auto transform = car->attributes.transform;
        const auto front = glm::normalize(glm::vec3(transform[2]));

        // rotate the camera to face the direction of the car
        camera->setFront(front);
        camera->setRight(glm::normalize(glm::cross(front, glm::vec3(0.0F, 1.0F, 0.0F))));
        camera->setUp(glm::normalize(glm::cross(camera->getRight(), front)));

        return;
    }

    if (mode == Mode::DRIVE) {
        car->update(dt);
        car->attributes.position = attributes.position;

        const auto transform = car->attributes.transform;
        const auto front = glm::normalize(glm::vec3(transform[2]));

        car->attributes.position += front * 0.5F;

        camera->setPosition(attributes.position + glm::vec3(0.0F, 5.0F, 0.0F));

        return;
    }

    if (mode == Mode::ORBIT) {
        camera->circleOrbit(dt);
        return;
    }

    if (mode == Mode::FIXED) {
        return;
    }

    camera->setPosition(attributes.position + glm::vec3(0.0F, 8.0F, 0.0F));

    const glm::vec3 front = camera->getFront();
    const glm::vec3 modelForward = glm::normalize(glm::vec3(attributes.transform[2]));

    auto rotationRequired = glm::vec3(0.0F);

    const float dotProduct = dot(modelForward, front);

    if (dotProduct < 0.99F) {
        const float angle = std::acos(dotProduct);
        const glm::vec3 axis = normalize(cross(modelForward, front));
        rotationRequired = axis * angle;
    }

    if (length(rotationRequired) > 0.01F) {
        rotationRequired.z = 0.0F;
        rotationRequired.x = 0.0F;

        attributes.applyRotation(rotationRequired);
    }

    camera->update(dt);
}

[[nodiscard]] auto Player::shouldDraw() const -> bool {
    return drawModel;
}

void Player::shouldDraw(const bool draw) {
    drawModel = draw;
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

auto Player::getMode() const -> Mode {
    return mode;
}

auto Player::getCar() const -> std::shared_ptr<BumperCar> {
    return car;
}

