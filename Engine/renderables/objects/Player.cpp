//
// Created by Jacob Edwards on 13/03/2024.
//

#include "Player.h"

#include <App.h>

#include "BumperCar.h"
#include "Config.h"
#include "graphics/Color.h"
#include "graphics/Shader.h"
#include "imgui/imgui.h"
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <print>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <utils/ShaderManager.h>

#include "utils/Camera.h"
#include "renderables/Entity.h"
#include "renderables/Particle.h"

Player::Player(const Mode mode) : Entity("../Assets/objects/person/person2.obj") {
    this->mode = mode;
    shader = ShaderManager::GetInstance().get("Untextured");

    auto carModelMat = Config::IDENTITY_MATRIX;
    carModelMat = glm::translate(carModelMat, glm::vec3(0.0F, 10.0F, 0.0F));

    switch (mode) {
        case Mode::FPS:
            camera.setMode(Camera::Mode::FPS);
            break;
        case Mode::ORBIT:
            attributes.gravityAffected = false;
            camera.setMode(Camera::Mode::ORBIT);
            break;
        case Mode::FREE:
            attributes.gravityAffected = false;
            camera.setMode(Camera::Mode::FREE);
            break;
        case Mode::FIXED:
            attributes.gravityAffected = false;
            camera.setMode(Camera::Mode::FIXED);
            break;
        case Mode::PATH:
            attributes.gravityAffected = false;
            camera.setMode(Camera::Mode::PATH);
            camera.setPitchLimits(-45.0F, 45.0F);
            camera.setYawLimits(45.0F, 135.0F);
            camera.setYaw(90.0F);
            car = std::make_shared<BumperCar>();
            car->setMode(BumperCar::Mode::PATHED);
            car->setIsPlayer(true);
            box = car->getBoundingBox();
            isDriving = true;
            break;
        case Mode::DRIVE:
            attributes.gravityAffected = false;
            camera.setMode(Camera::Mode::DRIVE);
            camera.setPitchLimits(-45.0F, 45.0F);
            camera.setYawLimits(45.0F, 135.0F);
            camera.setYaw(90.0F);
            car = std::make_shared<BumperCar>();
            car->setMode(BumperCar::Mode::PATHED);
            car->setIsPlayer(true);
            box = car->getBoundingBox();
            isDriving = true;
            break;
        case Mode::DUEL:
            attributes.gravityAffected = true;
            camera.setMode(Camera::Mode::FPS);
            car = std::make_shared<BumperCar>();
            car->setMode(BumperCar::Mode::NONE);
            car->shouldDrawPlayer(false);
    }
}


void Player::processKeyboard(const Direction direction, const float /*deltaTime*/) {
    if (mode == Mode::ORBIT || mode == Mode::PATH || mode == Mode::FIXED) {
        return;
    }

    const glm::vec3 front = mode == Mode::DRIVE ? car->attributes.getFront() : camera.getFront();
    const auto right = glm::normalize(glm::cross(front, glm::vec3(0.0F, 1.0F, 0.0F)));

    if (!attributes.isGrounded && mode == Mode::FPS) {
        return;
    }

    if (mode == Mode::DRIVE && car->hasBroke()) {
        return;
    }

    switch
    (direction) {
        case Direction::FORWARD:
            if (mode == Mode::DRIVE) {
                car->attributes.applyForce(front * 50.0F);
            } else {
                attributes.applyForce(front * 10.0F);
            }
            break;
        case Direction::BACKWARD:
            if (mode == Mode::DRIVE) {
                car->attributes.applyForce(-front * 50.0F);
            } else {
                attributes.applyForce(-front * 10.0F);
            }
            break;
        case Direction::LEFT:
            if (mode == Mode::DRIVE) {
                constexpr auto rotation = glm::vec3(0.0F, 0.1F, 0.0F);
                car->attributes.applyRotation(rotation);
            } else {
                attributes.applyForce(-right * 10.0F);
            }
            break;
        case Direction::RIGHT:
            if (mode == Mode::DRIVE) {
                constexpr auto rotation = glm::vec3(0.0F, -0.1F, 0.0F);
                car->attributes.applyRotation(rotation);
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

[[nodiscard]] auto Player::getCamera() -> Camera & {
    return camera;
}

[[nodiscard]] auto Player::getPosition() const -> glm::vec3 {
    return attributes.position;
}

void Player::draw(const std::shared_ptr<Shader> shader) const {
    if (!drawModel) {
        return;
    }

    shader->use();
    shader->setUniform("color", Color::GREEN);
    Entity::draw(shader);
}

void Player::update(const float dt) {
    if (mode == Mode::PATH || mode == Mode::DRIVE || (mode == Mode::DUEL && isDriving)) {
        camera.update(dt);

        const glm::vec3 pos = car->attributes.position;
        const auto front = car->attributes.getFront();
        const auto up = car->attributes.getUp();
        const auto right = car->attributes.getRight();

        attributes.position = pos;

        const glm::vec3 backTranslation = thirdPersonMode ? -front * 30.0F : -front * 2.0F;
        const auto upTranslation = thirdPersonMode ? glm::vec3(0.0F, 12.0F, 0.0F) : up * 6.0F;

        attributes.position += backTranslation + upTranslation;
        camera.setPosition(attributes.position);

        const auto angle = glm::acos(glm::dot(front, glm::vec3(0.0F, 0.0F, 1.0F)));
        const auto axis = glm::cross(front, glm::vec3(0.0F, 0.0F, 1.0F));

        camera.rotate(-angle, axis);
        camera.setUp(up);
        camera.setRight(right);

        if (thirdPersonMode) {
            camera.setTarget(pos);
        }
        attributes.update(dt);

        return;
    }

    Entity::update(dt);

    if (mode == Mode::ORBIT) {
        camera.circleOrbit(dt);
        return;
    }

    if (mode == Mode::FIXED) {
        return;
    }

    camera.setPosition(attributes.position + glm::vec3(0.0F, 8.0F, 0.0F));

    const glm::vec3 front = camera.getFront();
    const glm::vec3 modelForward = attributes.getFront();

    auto rotationRequired = glm::vec3(0.0F);

    if (const float dotProduct = dot(modelForward, front); dotProduct < 0.99F) {
        const float angle = std::acos(dotProduct);
        const glm::vec3 axis = normalize(cross(modelForward, front));
        rotationRequired = axis * angle;
    }

    if (length(rotationRequired) > 0.01F) {
        rotationRequired.z = 0.0F;
        rotationRequired.x = 0.0F;

        attributes.applyRotation(rotationRequired);
    }

    camera.update(dt);
}

void Player::startDriving(const bool should) {
    if (should) {
        mode = Mode::DRIVE;
        attributes.gravityAffected = false;
        camera.setMode(Camera::Mode::DRIVE);
        camera.setPitchLimits(-45.0F, 45.0F);
        camera.setYawLimits(45.0F, 135.0F);
        camera.setYaw(90.0F);
        car = std::make_shared<BumperCar>();
        car->setMode(BumperCar::Mode::PLAYER);
        car->setIsPlayer(true);
        box = car->getBoundingBox();
        isDriving = true;
        car->isCurrentPlayer(true);
    }
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

auto Player::isThirdPerson() const -> bool {
    return thirdPersonMode;
}

void Player::collisionResponse() {
    if (car) {
        car->collisionResponse();
    }

    if (mode == Mode::FPS || mode == Mode::DRIVE || mode == Mode::PATH || mode == Mode::DUEL) {
        if (const auto force = glm::length(attributes.force); force > 100.0F) {
            App::view.blurScreen();
        }
    }

    Entity::collisionResponse();
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
    ImGui::SliderFloat("Speed", &speed, 0.0F, 100.0F);
    ImGui::SliderFloat("Nitro Force", &nitroForce, 0.0F, 1000.0F);
    if (mode == Mode::DRIVE || mode == Mode::PATH) {
        if (ImGui::Checkbox("Third Person Mode", &thirdPersonMode)) {
            camera.isThirdPerson(thirdPersonMode);
        }
    }
    ImGui::End();
}

void Player::nitro() {
    if (mode != Mode::DRIVE) {
        attributes.applyForce(camera.getFront() * nitroForce);
    } else {
        car->setNitro(true);
    }
}

auto Player::getMode() const -> Mode {
    return mode;
}

auto Player::getCar() const -> std::shared_ptr<BumperCar> {
    return car;
}

