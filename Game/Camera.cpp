//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Camera.h"


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
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(const Direction direction, const float deltaTime) {
    const float curAcceleration = acceleration * deltaTime;

    auto targetVelocity = glm::vec3(0.0F);

    switch (direction) {
        case Direction::FORWARD:
            targetVelocity += front;
            break;
        case Direction::BACKWARD:
            targetVelocity -= front;
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
        newVelocity = glm::normalize(newVelocity) * maxSpeed;
    }

    velocity = glm::mix(velocity, newVelocity, smoothing);

    // velocity += targetVelocity * curAcceleration;


    const glm::vec3 newPos = position + velocity * deltaTime;
    position = glm::mix(position, newPos, smoothing);

    velocity *= damping;

    if (fps) {
        position.y = yPosition;

        downwards ? yPosition -= 0.0001 : yPosition += 0.0001;

        if (yPosition > 0.05) {
            downwards = true;
        }

        if (yPosition < 0.0) {
            downwards = false;
        }
    }
}

void Camera::processMouseMovement(float xOffset, float yOffset, const GLboolean constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch != 0U) {
        if (pitch > MAXPITCH) {
            pitch = MAXPITCH;
        }
        if (pitch < MINPITCH) {
            pitch = MINPITCH;
        }
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(const float yOffset) {
    zoom -= yOffset;
    if (zoom < MINZOOM) { zoom = MINZOOM; }
    if (zoom > MAXZOOM) { zoom = MAXZOOM; }
}

void Camera::setFPS(const bool value) {
    fps = value;
}

[[nodiscard]] auto Camera::isFPS() const -> bool {
    return fps;
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

void Camera::updateCameraVectors() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw) * cos(glm::radians(pitch)));

    front = normalize(newFront);
    right = normalize(glm::cross(front, worldUp));
    up = normalize(glm::cross(right, front));
}
