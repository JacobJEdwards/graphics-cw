//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Camera.h"
#include <cmath>

#include <algorithm>
#include <glm/common.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include "imgui/imgui.h"


Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch, float damping, float smoothing,
               float acceleration, float maxspeed) : position(position), worldUp(worldUp), yaw(yaw), pitch(pitch),
                                                     damping(damping), smoothing(smoothing), acceleration(acceleration),
                                                     maxSpeed(maxspeed) {
    updateCameraVectors();
}

// constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw,
               float pitch) : position(glm::vec3(posX, posY, posZ)), worldUp(glm::vec3(upX, upY, upZ)), yaw(yaw),
                              pitch(pitch) {
    acceleration = ACCELERATION;
    maxSpeed = MAXSPEED;
    damping = DAMPING;
    smoothing = SMOOTHING;
    updateCameraVectors();
}

[[nodiscard]] auto Camera::getViewMatrix() const -> glm::mat4 {
    switch (mode) {
        case Mode::ORBIT:
            return lookAt(position, orbitTarget, worldUp);
        default:
            return lookAt(position, position + front, up);
    }
}

/*
[[nodiscard]] auto Camera::getViewMatrix() const -> glm::mat4 {
    const glm::vec3 pos = orbitTarget - front * orbitRadius;
    switch (mode) {
        case Mode::ORBIT:
            return lookAt(pos, orbitTarget, up);
        default:
            return lookAt(position, position + front, up);
    }
}

 */

void Camera::processKeyboard(const Direction direction, const float deltaTime) {
    const float curAcceleration = acceleration * deltaTime * 10.0F;

    auto targetVelocity = glm::vec3(0.0F);

    switch (direction) {
        case Direction::FORWARD:
            targetVelocity += front;
            if (mode == Mode::ORBIT) {
                orbitTarget += glm::vec3(0.0F, 0.1F, 0.0F);
            }
            break;
        case Direction::BACKWARD:
            targetVelocity -= front;
            if (mode == Mode::ORBIT) {
                orbitTarget += glm::vec3(0.0F, -0.1F, 0.0F);
                orbitTarget.y = std::max(orbitTarget.y, 0.0F);
            }
            break;
        case Direction::LEFT:
            targetVelocity -= right;
            break;
        case Direction::RIGHT:
            targetVelocity += right;
            break;
        case Direction::NONE:
            break;
    }

    glm::vec3 newVelocity = velocity + targetVelocity * curAcceleration;

    if (length(newVelocity) > maxSpeed) {
        newVelocity = normalize(newVelocity) * maxSpeed;
    }


    velocity = mix(velocity, newVelocity, smoothing);

    updatePosition(deltaTime);

    if (mode == Mode::FPS) {
        applyFPSModeEffects();
    }
}


void Camera::updatePosition(const float deltaTime) {
    glm::vec3 newPos = position + velocity * deltaTime;

    if (mode == Mode::ORBIT) {
        adjustOrbitPosition(newPos);
    }

    position = mix(position, newPos, smoothing);

    velocity *= damping;
}

void Camera::adjustOrbitPosition(glm::vec3 &newPos) const {
    const glm::vec3 dirToTarget = orbitTarget - newPos;
    newPos = orbitTarget - normalize(dirToTarget) * orbitRadius;
}

void Camera::circleOrbit(const float deltaTime) {
    if (!(mode == Mode::ORBIT)) {
        return;
    }

    orbitAngle += deltaTime * orbitSpeed;
    updateOrbitPosition();
}

void Camera::applyFPSModeEffects() {
    position.y = yPosition;

    downwards ? yPosition -= 0.0001 : yPosition += 0.0001;

    velocity *= damping;

    if (yPosition > 0.03) {
        downwards = true;
    }

    if (yPosition < 0.0) {
        downwards = false;
    }
}

void Camera::processMouseMovement(float xOffset, float yOffset, const GLboolean constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch != 0U) {
        pitch = std::min(pitch, MAXPITCH);
        pitch = std::max(pitch, MINPITCH);
    }

    updateCameraVectors();

    if (mode == Mode::ORBIT) {
        orbitAngle += xOffset * orbitSpeed * 0.1F;
        orbitHeight += yOffset * orbitSpeed * 0.1F;
        orbitHeight = std::max(orbitHeight, yPosition);
        orbitHeight = std::min(orbitHeight, 10.0F);
        updateOrbitPosition();
    }
}

void Camera::processMouseScroll(const float yOffset) {
    zoom -= yOffset;
    zoom = std::max(zoom, MINZOOM);
    zoom = std::min(zoom, MAXZOOM);
}

void Camera::setMode(const Mode value) {
    mode = value;
}

void Camera::setOrbit(const glm::vec3 target, const float radius, const float angle, const float speed) {
    orbitTarget = target;
    orbitRadius = radius;
    orbitAngle = angle;
    orbitSpeed = speed;
}

[[nodiscard]] auto Camera::getMode() const -> Mode {
    return mode;
}

[[nodiscard]] auto Camera::getZoom() const -> float {
    return zoom;
}

[[nodiscard]] auto Camera::getPosition() const -> glm::vec3 {
    return position;
}

[[nodiscard]] auto Camera::getFront() const -> glm::vec3 {
    return front;
}

[[nodiscard]] auto Camera::getUp() const -> glm::vec3 {
    return up;
}

[[nodiscard]] auto Camera::getRight() const -> glm::vec3 {
    return right;
}

[[nodiscard]] auto Camera::getWorldUp() const -> glm::vec3 {
    return worldUp;
}

void Camera::setPosition(const glm::vec3 &position) {
    this->position = position;
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 newFront;

    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw) * cos(glm::radians(pitch)));

    front = normalize(newFront);
    right = normalize(cross(front, worldUp));
    up = normalize(cross(right, front));

    if (mode == Mode::ORBIT) {
        updateOrbitPosition();
    }
}

void Camera::updateOrbitPosition() {
    position.x = orbitTarget.x + orbitRadius * cos(glm::radians(orbitAngle));
    position.z = orbitTarget.z + orbitRadius * sin(glm::radians(orbitAngle));
    position.y = orbitTarget.y + orbitHeight;
}

void Camera::controlInterface() {
    ImGui::Begin("Camera Options");
    ImGui::SeparatorText("General");
    ImGui::SliderFloat("Zoom", &zoom, MINZOOM, MAXZOOM);
    ImGui::SliderFloat("Movement Speed", &movementSpeed, 0.0F, MAXSPEED);
    ImGui::SliderFloat("Sensitivity", &mouseSensitivity, 0.0F, 2.5F);

    ImGui::SeparatorText("Orbit");
    ImGui::SliderFloat("Orbit Radius", &orbitRadius, 0.0F, 100.0F);
    ImGui::SliderFloat("Orbit Height", &orbitHeight, 0.0F, 100.0F);
    ImGui::End();
}

void Camera::modeInterface() {
    int checked = getModeInt();

    ImGui::Begin("Camera Mode");
    ImGui::RadioButton("Orbit", &checked, 0);
    ImGui::RadioButton("Free", &checked, 1);
    ImGui::RadioButton("FPS", &checked, 2);
    ImGui::End();

    switch (checked) {
        case 0:
            mode = Mode::ORBIT;
            break;
        case 1:
            mode = Mode::FREE;
            break;
        case 2:
            mode = Mode::FPS;
            break;
        default:
            break;
    }
}

[[nodiscard]] auto Camera::getModeInt() const -> int {
    switch (mode) {
        case Mode::ORBIT:
            return 0;
        case Mode::FREE:
            return 1;
        case Mode::FPS:
            return 2;
        default:
            return -1;
    }
}
