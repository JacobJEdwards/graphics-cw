//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_CAMERA_H
#define CW_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    enum class Direction {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    explicit Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : position(position),  worldUp(up), yaw(yaw), pitch(pitch) {
        this->front = glm::vec3(0.0f, 0.0f, -1.0f);
        this->movementSpeed = SPEED;
        this->mouseSensitivity = SENSITIVITY;
        this->zoom = ZOOM;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : position(glm::vec3(posX, posY, posZ)), worldUp(glm::vec3(upX, upY, upZ)), yaw(yaw), pitch(pitch) {
        this->front = glm::vec3(0.0f, 0.0f, -1.0f);
        this->zoom = ZOOM;
        this->mouseSensitivity = SENSITIVITY;
        this->movementSpeed = SPEED;

        updateCameraVectors();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    void processKeyboard(Direction direction, float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        if (direction == Direction::FORWARD) {
            position += front * velocity;
        }
        if (direction == Direction::BACKWARD) {
            position -= front * velocity;
        }
        if (direction == Direction::LEFT) {
            position -= right * velocity;
        }
        if (direction == Direction::RIGHT) {
            position += right * velocity;
        }
        if (fps) {
            position.y = yPosition;
        }
    }

    void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true) {
        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;

        yaw += xOffset;
        pitch += yOffset;

        if (constrainPitch) {
            if (pitch > 89.0f) {
                pitch = 89.0f;
            }
            if (pitch < -89.0f) {
                pitch = -89.0f;
            }
        }

        updateCameraVectors();
    }

    void processMouseScroll(float yOffset) {
        zoom -= yOffset;
        if (zoom < 1.0f) {zoom = 1.0f;}
        if (zoom > 45.0f) {zoom = 45.0f;}
    }

    void setFPS(bool value) {
        fps = value;
    }

    bool isFPS() const {
        return fps;
    }

private:
    bool fps = false;
    float yPosition = 0.0f;

    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw) * cos(glm::radians(pitch)));

        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};


#endif //CW_CAMERA_H
