//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <glm/ext/vector_float3.hpp>
#include <memory>

#include "BumperCar.h"
#include "utils/Camera.h"
#include "Entity.h"

class Player final : public Entity {
public:
    enum class Mode {
        FPS, FREE, ORBIT, FIXED, PATH, DRIVE
    };

    enum class Direction {
        FORWARD, BACKWARD, LEFT, RIGHT, NONE, UP, DOWN
    };

    explicit Player(Mode mode = Mode::FPS);

    void processKeyboard(Direction direction, float deltaTime);

    [[nodiscard]] auto getCamera() const -> Camera &;

    [[nodiscard]] auto getPosition() const -> glm::vec3;

    void update(float dt) override;

    void jump();

    [[nodiscard]] auto shouldDraw() const -> bool;

    void shouldDraw(bool draw);

    void interface();

    void debug() const;

    void nitro();

    [[nodiscard]] auto getMode() const -> Mode;

    [[nodiscard]] auto getCar() const -> std::shared_ptr<BumperCar>;

private:
    Mode mode = Mode::FPS;

    bool drawModel = true;
    float jumpForce = 100.0F;

    std::unique_ptr<Camera> camera =
            std::make_unique<Camera>(glm::vec3(0.0F, 5.0F, 3.0F));

    std::shared_ptr<BumperCar> car = nullptr;
};

#endif // PLAYER_H
