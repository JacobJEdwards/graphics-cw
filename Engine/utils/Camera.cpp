//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Camera.h"
#include <array>
#include <cmath>

#include <algorithm>
#include <glm/matrix.hpp>
#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "imgui/imgui.h"

Camera::Camera(const glm::vec3 position, const glm::vec3 worldUp, const float yaw, const float pitch)
    : position(position), worldUp(worldUp), yaw(yaw), pitch(pitch) {
    updateCameraVectors();
}

[[nodiscard]] auto Camera::getOrthoMatrix() const -> glm::mat4 {
    // match perspective matrix
    const float fov = zoom;

    return glm::ortho(-aspect * fov, aspect * fov, -fov, fov, nearPlane, renderDistance);
}

[[nodiscard]] auto Camera::getViewMatrix() const -> glm::mat4 {
    switch (mode) {
        case Mode::FPS:
        case Mode::FREE:
        case Mode::DRIVE:
        case Mode::PATH:
            if (thirdPersonMode) {
                return glm::lookAt(position, target + front, worldUp);
            }

            return lookAt(position, position + front, up);
        case Mode::ORBIT:
        case Mode::FIXED:
        default:
            return lookAt(position, target + front, worldUp);
    }
}

auto Camera::getFrustumCorners() const -> std::array<glm::vec3, 4> {
    const auto view = getViewMatrix();
    const auto projection = getProjectionMatrix();

    const auto inverseViewProjection = inverse(projection * view);

    std::array<glm::vec3, 4> corners{};

    for (int i = 0; i < 4; i++) {
        const auto corner = glm::vec4((i < 2) ? -1.0F : 1.0F, (i % 2 == 0) ? -1.0F : 1.0F, -1.0F, 1.0F);
        const auto worldSpace = inverseViewProjection * corner;
        corners[i] = glm::vec3(worldSpace) / worldSpace.w;
    }

    return corners;
}

auto Camera::getLightViewMatrix(const glm::vec3 lightDirection) const -> glm::mat4 {
    auto centre = glm::vec3(0.0F);

    const auto corners = getFrustumCorners();

    std::ranges::for_each(corners, [&centre](auto &corner) { centre += corner; });
    centre /= corners.size();

    return glm::lookAt(centre - lightDirection, centre, worldUp);
}

void Camera::setTarget(const glm::vec3 target) {
    this->target = target;
}


[[nodiscard]] auto Camera::getRenderDistance() const -> float {
    return renderDistance;
}

[[nodiscard]] auto Camera::getProjectionMatrix() const -> glm::mat4 {
    return glm::perspective(glm::radians(zoom), aspect, nearPlane, renderDistance);
}

[[nodiscard]] auto Camera::getProjectionDepthMatrix() const -> glm::mat4 {
    return glm::perspective(glm::radians(zoom), aspect, nearPlaneDepth, renderDistanceDepth);
}


void Camera::circleOrbit(const float deltaTime) {
    orbitAngle += deltaTime * orbitSpeed;
    updateOrbitPosition();
}

void Camera::processMouseMovement(float xOffset, float yOffset) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    pitch = std::min(pitch, maxPitch);
    pitch = std::max(pitch, minPitch);

    // contrain yaw with max and min
    yaw = std::min(yaw, maxYaw);
    yaw = std::max(yaw, minYaw);

    yaw = std::fmod(yaw, 360.0F);

    if (mode == Mode::ORBIT) {
        orbitAngle += xOffset * orbitSpeed * 0.1F;
        orbitHeight += yOffset * orbitSpeed * 0.1F;
        orbitHeight = std::max(orbitHeight, yPosition);
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(const float yOffset) {
    zoom -= yOffset;
    zoom = std::max(zoom, MINZOOM);
    zoom = std::min(zoom, MAXZOOM);
}

void Camera::setMode(const Mode value) { mode = value; }

void Camera::setOrbit(const glm::vec3 target, const float radius, const float angle, const float speed,
                      const float height) {
    this->target = target;
    orbitRadius = radius;
    orbitAngle = angle;
    orbitSpeed = speed;
    orbitHeight = height;
    distance = glm::distance(target, position);
}

void Camera::setFixed(const glm::vec3 target, const glm::vec3 position) {
    this->target = target;
    this->position = position;
    distance = glm::distance(target, position);
}

[[nodiscard]] auto Camera::getMode() const -> Mode { return mode; }

[[nodiscard]] auto Camera::getZoom() const -> float { return zoom; }

[[nodiscard]] auto Camera::getPosition() const -> glm::vec3 { return position; }

[[nodiscard]] auto Camera::getFront() const -> glm::vec3 {
    if (mode == Mode::ORBIT || thirdPersonMode || mode == Mode::FIXED) {
        return normalize(target - position);
    }
    return front;
}

[[nodiscard]] auto Camera::getUp() const -> glm::vec3 {
    /*
        if (thirdPersonMode) {
            return worldUp;
        }
        */

    if (mode == Mode::FIXED) {
        return glm::normalize(cross(getRight(), getFront()));
    }

    return up;
}

[[nodiscard]] auto Camera::getRight() const -> glm::vec3 {
    if (mode == Mode::ORBIT || thirdPersonMode) {
        return normalize(cross(getFront(), getUp()));
    }

    return right;
}

[[nodiscard]] auto Camera::getWorldUp() const -> glm::vec3 { return worldUp; }

[[nodiscard]] auto Camera::getYaw() const -> float { return yaw; }

void Camera::setPosition(const glm::vec3 &position) {
    Camera::position = position;
    updateCameraVectors();
}

void Camera::setFront(const glm::vec3 &front) {
    Camera::front = front;
}

void Camera::setUp(const glm::vec3 &up) {
    Camera::up = up;
}

void Camera::setRight(const glm::vec3 &right) {
    Camera::right = right;
}

void Camera::setPitch(const float pitch) {
    Camera::pitch = pitch;
    updateCameraVectors();
}

void Camera::setPitchLimits(const float min, const float max) {
    minPitch = min;
    maxPitch = max;
}

void Camera::setYawLimits(const float min, const float max) {
    minYaw = min;
    maxYaw = max;
}

void Camera::setYaw(const float yaw) {
    Camera::yaw = yaw;
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
    position.x = target.x + orbitRadius * cos(glm::radians(orbitAngle)) * cos(glm::radians(orbitHeight));
    position.z = target.z + orbitRadius * sin(glm::radians(orbitAngle)) * cos(glm::radians(orbitHeight));
    position.y = orbitHeight;
}

void Camera::interface() {
    bool hasChanged = false;

    ImGui::Begin("Camera Options");

    ImGui::SeparatorText("General");
    hasChanged |= ImGui::SliderFloat("Zoom", &zoom, MINZOOM, MAXZOOM);
    hasChanged |= ImGui::SliderFloat("Sensitivity", &mouseSensitivity, 0.0F, 2.5F);
    hasChanged |= ImGui::SliderFloat("Render Distance", &renderDistance, 1.0F, 10000.0F);
    hasChanged |= ImGui::SliderFloat("Near Plane", &nearPlane, 0.1F, 2.5F);
    hasChanged |= ImGui::SliderFloat("Render Distance Depth", &renderDistanceDepth, 1.0F, 1000000.0F);
    hasChanged |= ImGui::SliderFloat("Near Plane Depth", &nearPlaneDepth, 0.1F, 200.0F);


    if (mode == Mode::ORBIT) {
        ImGui::SeparatorText("Orbit");
        hasChanged |= ImGui::SliderFloat("Orbit Radius", &orbitRadius, 0.0F, 100.0F);
        hasChanged |= ImGui::SliderFloat("Orbit Height", &orbitHeight, 0.0F, 100.0F);
        hasChanged |= ImGui::SliderFloat("Orbit Speed", &orbitSpeed, 0.0F, 100.0F);
    }

    if (mode == Mode::FIXED) {
        ImGui::SeparatorText("Fixed");
        if (ImGui::SliderFloat("Distance", &distance, 0.0F, 100.0F)) {
            position.x = target.x + distance * std::cos(glm::radians(yaw));
            position.z = target.z + distance * std::sin(glm::radians(yaw));
        }
    }

    ImGui::End();

    if (hasChanged) {
        updateCameraVectors();
    }
}

void Camera::setAspect(const float aspect) { this->aspect = aspect; }

void Camera::update(const float deltaTime) {
    if (mode == Mode::ORBIT) {
        circleOrbit(deltaTime);
    }
}

auto Camera::getPitch() const -> float {
    return pitch;
}

auto Camera::getNear() const -> float {
    return nearPlane;
}

void Camera::rotate(const float angle, const glm::vec3 &axis) {
    if (angle == 0.0F || glm::length(axis) == 0.0F) {
        return;
    }

    const auto rotation = glm::rotate(glm::mat4(1.0F), angle, axis);
    front = glm::vec3(rotation * glm::vec4(front, 1.0F));
    right = glm::vec3(rotation * glm::vec4(right, 1.0F));
    up = glm::vec3(rotation * glm::vec4(up, 1.0F));
}

void Camera::rotate(const glm::vec3 &rotation) {
    const auto xRotation = glm::rotate(glm::mat4(1.0F), rotation.x, glm::vec3(1.0F, 0.0F, 0.0F));
    const auto yRotation = glm::rotate(glm::mat4(1.0F), rotation.y, glm::vec3(0.0F, 1.0F, 0.0F));
    const auto zRotation = glm::rotate(glm::mat4(1.0F), rotation.z, glm::vec3(0.0F, 0.0F, 1.0F));

    front = glm::vec3(zRotation * yRotation * xRotation * glm::vec4(front, 1.0F));
    right = glm::vec3(zRotation * yRotation * xRotation * glm::vec4(right, 1.0F));
    up = glm::vec3(zRotation * yRotation * xRotation * glm::vec4(up, 1.0F));
}

void Camera::isThirdPerson(const bool thirdPersonMode) {
    this->thirdPersonMode = thirdPersonMode;
}
