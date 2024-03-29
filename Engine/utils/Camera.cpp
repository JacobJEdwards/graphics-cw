//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Camera.h"
#include <cmath>

#include <algorithm>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

#include "imgui/imgui.h"

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
        : position(position), worldUp(worldUp), yaw(yaw), pitch(pitch) {

    updateCameraVectors();
}

// constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
        : position(glm::vec3(posX, posY, posZ)), worldUp(glm::vec3(upX, upY, upZ)),
          yaw(yaw), pitch(pitch) {
    updateCameraVectors();
}

[[nodiscard]] auto Camera::getViewMatrix() const -> glm::mat4 {
    switch (mode) {
        case Mode::ORBIT:
            return lookAt(position, target, worldUp);
        case Mode::FPS:
            return lookAt(position, position + front, up);
        case Mode::FREE:
            return lookAt(position, position + front, up);
        case Mode::FIXED:
            return lookAt(position, target, up);

        default:
            return lookAt(position, position + front, up);
    }
}

[[nodiscard]] auto Camera::getRenderDistance() const -> float {
    return renderDistance;
}

[[nodiscard]] auto Camera::getProjectionMatrix() const -> glm::mat4 {
    return glm::perspective(glm::radians(zoom), aspect, nearPlane, renderDistance);
}

void Camera::circleOrbit(const float deltaTime) {
    orbitAngle += deltaTime * orbitSpeed;
    updateOrbitPosition();
}

/*
void Camera::applyFPSModeEffects() {
  if (!attributes.isGrounded) {
    attributes.applyForce(Physics::GRAVITY_VECTOR * attributes.mass);
  }
  downwards ? yPosition -= 0.0001 : yPosition += 0.0001;

  if (yPosition > 0.03) {
    downwards = true;
  }

  if (yPosition < 0.0) {
    downwards = false;
  }
}
*/

void Camera::processMouseMovement(float xOffset, float yOffset,
                                  const GLboolean constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch != 0U) {
        if (mode == Mode::FIXED) {
            pitch = std::min(pitch, 89.0F);
            pitch = std::max(pitch, -89.0F);
        } else {
            pitch = std::min(pitch, MAXPITCH);
            pitch = std::max(pitch, MINPITCH);
        }
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

void Camera::setMode(const Mode value) { mode = value; }

void Camera::setOrbit(const glm::vec3 target, const float radius,
                      const float angle, const float speed) {
    this->target = target;
    orbitRadius = radius;
    orbitAngle = angle;
    orbitSpeed = speed;
}

void Camera::setOrbit(glm::vec3 target, float radius, float angle, float speed,
                      float height) {
    this->target = target;
    orbitRadius = radius;
    orbitAngle = angle;
    orbitSpeed = speed;
    orbitHeight = height;
}

void Camera::setFixed(glm::vec3 target, glm::vec3 position) {
    this->target = target;
    this->position = position;
    distance = glm::distance(target, position);
}

[[nodiscard]] auto Camera::getMode() const -> Mode { return mode; }

[[nodiscard]] auto Camera::getZoom() const -> float { return zoom; }

[[nodiscard]] auto Camera::getPosition() const -> glm::vec3 { return position; }

[[nodiscard]] auto Camera::getFront() const -> glm::vec3 { return front; }

[[nodiscard]] auto Camera::getUp() const -> glm::vec3 { return up; }

[[nodiscard]] auto Camera::getRight() const -> glm::vec3 { return right; }

[[nodiscard]] auto Camera::getWorldUp() const -> glm::vec3 { return worldUp; }

[[nodiscard]] auto Camera::getYaw() const -> float { return yaw; }

void Camera::setPosition(const glm::vec3 &position) {
    Camera::position = position;
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
    position.x = target.x + orbitRadius * cos(glm::radians(orbitAngle));
    position.z = target.z + orbitRadius * sin(glm::radians(orbitAngle));
    position.y = target.y + orbitHeight;
}

void Camera::interface() {
    ImGui::Begin("Camera Options");
    ImGui::SeparatorText("General");
    ImGui::SliderFloat("Zoom", &zoom, MINZOOM, MAXZOOM);
    ImGui::SliderFloat("Sensitivity", &mouseSensitivity, 0.0F, 2.5F);
    ImGui::SliderFloat("Render Distance", &renderDistance, 1.0F, 1000.0F);
    ImGui::SliderFloat("Near Plane", &nearPlane, 0.1F, 2.5F);

    if (mode == Mode::ORBIT) {
        ImGui::SeparatorText("Orbit");
        ImGui::SliderFloat("Orbit Radius", &orbitRadius, 0.0F, 100.0F);
        ImGui::SliderFloat("Orbit Height", &orbitHeight, 0.0F, 100.0F);
    }

    if (mode == Mode::FIXED) {
        ImGui::SeparatorText("Fixed");
        ImGui::SliderFloat("Distance", &distance, 0.0F, 100.0F);

        // calculate the new position
        position.x = target.x + distance * cos(glm::radians(yaw));
        position.z = target.z + distance * sin(glm::radians(yaw));
    }

    // check for changes
    updateCameraVectors();

    ImGui::End();
}

void Camera::setAspect(const float aspect) { this->aspect = aspect; }

void Camera::update(float dt) {
    if (mode == Mode::ORBIT) {
        circleOrbit(dt);
    }
}

auto Camera::getPitch() const -> float {
    return pitch;
}

auto Camera::getNear() const -> float {
    return nearPlane;
}
