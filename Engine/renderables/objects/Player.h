//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <glm/ext/vector_float3.hpp>
#include <memory>

#include "BumperCar.h"
#include "graphics/Shader.h"
#include "utils/Camera.h"
#include "renderables/Entity.h"

class Player final : public Entity {
public:
    using Entity::draw;

    enum class Mode {
        FPS, FREE, ORBIT, FIXED, PATH, DRIVE, DUEL
    };

    enum class Direction {
        FORWARD, BACKWARD, LEFT, RIGHT, NONE, UP, DOWN
    };

    explicit Player(Mode mode = Mode::FPS);

    void draw(std::shared_ptr<Shader> shader) const override;

    void processKeyboard(Direction direction, float deltaTime);

    [[nodiscard]] auto getCamera() -> Camera &;

    [[nodiscard]] auto getPosition() const -> glm::vec3;

    void update(float dt) override;

    void jump();

    void startDriving(bool should);

    [[nodiscard]] auto shouldDraw() const -> bool;

    void shouldDraw(bool draw);

    void interface();

    void debug() const;

    void nitro();

    [[nodiscard]] auto getMode() const -> Mode;

    [[nodiscard]] auto getCar() const -> std::shared_ptr<BumperCar>;

    [[nodiscard]] auto isThirdPerson() const -> bool;

private:
    Mode mode = Mode::FPS;

    bool drawModel = false;
    float jumpForce = 100.0F;

    float nitroForce = 250.0F;
    float nitroDuration = 0.0F;
    float nitroMaxDuration = 5.0F;
    bool nitroActive = false;

    float speed = 10.0F;

    bool isDriving = false;

    bool thirdPersonMode = false;

    Camera camera{glm::vec3(0.0F, 5.0F, 3.0F)};

    std::shared_ptr<BumperCar> car = nullptr;
};

#endif // PLAYER_H
