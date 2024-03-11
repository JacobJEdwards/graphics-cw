//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_CAMERA_H
#define CW_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

constexpr float YAW = -90.0F;
constexpr float PITCH = 0.0F;
constexpr float SPEED = 3.0F;
constexpr float SENSITIVITY = 0.1F;
constexpr float ZOOM = 45.0F;

constexpr float DAMPING = 0.99F;
constexpr float SMOOTHING = 0.9F;
constexpr float ACCELERATION = 5.0F;
constexpr float MAXSPEED = 10000.0F;

constexpr float MAXZOOM = 45.0F;
constexpr float MINZOOM = 1.0F;

constexpr float MAXPITCH = 89.0F;
constexpr float MINPITCH = -89.0F;

class Camera {
public:
    enum class Direction {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        NONE
    };

    enum class Mode {
        FPS,
        FREE,
        ORBIT,
        FIXED
    };

    explicit Camera(glm::vec3 position = glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3 worldUp = glm::vec3(0.0F, 1.0F, 0.0F),
                    float yaw = YAW, float pitch = PITCH, float damping = DAMPING, float smoothing = SMOOTHING,
                    float acceleration = ACCELERATION, float maxspeed = MAXSPEED);

    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    [[nodiscard]] auto getViewMatrix() const -> glm::mat4;

    void processKeyboard(Direction direction, float deltaTime);

    void processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = 1U);

    void processMouseScroll(float yOffset);

    void setFPS(bool value);

    [[nodiscard]] auto isFPS() const -> bool;

    [[nodiscard]] auto getZoom() const -> float;

    [[nodiscard]] auto getPosition() const -> glm::vec3;

    [[nodiscard]] auto getFront() const -> glm::vec3;

    [[nodiscard]] auto getUp() const -> glm::vec3;

    [[nodiscard]] auto getRight() const -> glm::vec3;

    [[nodiscard]] auto getWorldUp() const -> glm::vec3;

    void setPosition(const glm::vec3 &position);

private:

    Mode mode = Mode::FREE;

    glm::vec3 front = glm::vec3(0.0F, 0.0F, -1.0F);
    glm::vec3 up{};
    glm::vec3 right{};

    glm::vec3 position;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float damping;
    float smoothing;
    float acceleration;
    float maxSpeed;

    float movementSpeed = SPEED;
    float mouseSensitivity = SENSITIVITY;
    float zoom = ZOOM;

    glm::vec3 velocity = glm::vec3(0.0F, 0.0F, 0.0F);

    float yPosition = 0.0F;
    bool downwards = false;

    void updateCameraVectors();
};


#endif //CW_CAMERA_H
