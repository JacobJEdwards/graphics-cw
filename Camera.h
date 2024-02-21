//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_CAMERA_H
#define CW_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

#include <iostream>

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 3.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    enum class Direction {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        NONE
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
        float curAcceleration = acceleration * deltaTime;

        auto targetVelocity = glm::vec3(0.0f, 0.0f, 0.0f);

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

        if (glm::length(newVelocity) > maxSpeed) {
            newVelocity = glm::normalize(newVelocity) * maxSpeed;
        }

        velocity = glm::mix(velocity, newVelocity, 0.9f);

        //velocity += targetVelocity * curAcceleration;

        std::cout << "Velocity: " << glm::length(velocity) << std::endl;

        glm::vec3 newPos = position + velocity * deltaTime;
        position = glm::mix(position, newPos, 0.9f);

        velocity *= 0.9999f;

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

    float acceleration = 5.0f;
    float maxSpeed = 10000.0f;

    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);

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
