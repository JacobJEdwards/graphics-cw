//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_CAMERA_H
#define CW_CAMERA_H

#include <array>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>

#include <GL/glew.h>

constexpr float YAW = -90.0F;
constexpr float PITCH = 0.0F;
constexpr float SENSITIVITY = 0.1F;
constexpr float ZOOM = 45.0F;

constexpr float MAXZOOM = 180.0F;
constexpr float MINZOOM = 1.0F;

constexpr float MAXPITCH = 89.0F;
constexpr float MINPITCH = -89.0F;

constexpr float MAXYAW = 360.0F;
constexpr float MINYAW = -360.0F;

constexpr float MAXSPEED = 10.0F;
constexpr float MINSPEED = 0.0F;

constexpr float FARPLANE = 500.0F;
constexpr float NEARPLANE = 0.1F;


class Camera {
public:
    enum class Mode {
        FPS, FREE, ORBIT, FIXED, PATH, DRIVE
    };

    explicit Camera(glm::vec3 position = glm::vec3(0.0F, 0.0F, 0.0F),
                    glm::vec3 worldUp = glm::vec3(0.0F, 1.0F, 0.0F),
                    float yaw = YAW, float pitch = PITCH);

    [[nodiscard]] auto getViewMatrix() const -> glm::mat4;

    [[nodiscard]] auto getOrthoMatrix() const -> glm::mat4;

    [[nodiscard]] auto getFrustumCorners() const -> std::array<glm::vec3, 4>;

    [[nodiscard]] auto getLightViewMatrix(glm::vec3 lightDirection) const -> glm::mat4;

    void processMouseMovement(float xOffset, float yOffset);

    void processMouseScroll(float yOffset);

    [[nodiscard]] auto getProjectionDepthMatrix() const -> glm::mat4;

    void setMode(Mode value);

    void setOrbit(glm::vec3 target, float radius, float angle, float speed,
                  float height);

    void setFixed(glm::vec3 target, glm::vec3 position);

    [[nodiscard]] auto getMode() const -> Mode;

    [[nodiscard]] auto getZoom() const -> float;

    [[nodiscard]] auto getPosition() const -> glm::vec3;

    [[nodiscard]] auto getFront() const -> glm::vec3;

    [[nodiscard]] auto getUp() const -> glm::vec3;

    [[nodiscard]] auto getRight() const -> glm::vec3;

    [[nodiscard]] auto getWorldUp() const -> glm::vec3;

    [[nodiscard]] auto getRenderDistance() const -> float;

    [[nodiscard]] auto getProjectionMatrix() const -> glm::mat4;

    [[nodiscard]] auto getYaw() const -> float;

    [[nodiscard]] auto getPitch() const -> float;

    [[nodiscard]] auto getNear() const -> float;

    void setPosition(const glm::vec3 &position);

    void setFront(const glm::vec3 &front);

    void setUp(const glm::vec3 &up);

    void setRight(const glm::vec3 &right);

    void setYaw(float yaw);

    void setPitch(float pitch);

    void setPitchLimits(float min, float max);

    void setYawLimits(float min, float max);

    void circleOrbit(float deltaTime);

    void setAspect(float aspect);

    void interface();

    void update(float deltaTime);

    void rotate(float angle, const glm::vec3 &axis);

    void rotate(const glm::vec3 &rotation);

    void isThirdPerson(bool thirdPersonMode);

    void setTarget(glm::vec3 target);

private:
    Mode mode = Mode::FREE;

    glm::vec3 front = glm::vec3(0.0F, 0.0F, -1.0F);
    glm::vec3 up{};
    glm::vec3 right{};

    glm::vec3 position;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float maxPitch = MAXPITCH;
    float minPitch = MINPITCH;

    float maxYaw = MAXYAW;
    float minYaw = MINYAW;

    float mouseSensitivity = SENSITIVITY;
    float zoom = ZOOM;

    float renderDistance = FARPLANE;
    float nearPlane = NEARPLANE;

    float renderDistanceDepth = FARPLANE;
    float nearPlaneDepth = 10.0F;

    glm::vec3 target = glm::vec3(0.0F, 0.0F, 0.0F);

    float orbitRadius = 0.0F;
    float orbitAngle = 0.0F;
    float orbitSpeed = 0.0F;
    float orbitHeight = 0.0F;

    float distance = 0.0F;

    float aspect = 1.0F;

    float yPosition = 1.0F;

    bool downwards = false;

    bool thirdPersonMode = false;

    void updateCameraVectors();

    void updateOrbitPosition();
};

#endif // CW_CAMERA_H
