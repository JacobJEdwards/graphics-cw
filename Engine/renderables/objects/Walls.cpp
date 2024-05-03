//
// Created by Jacob Edwards on 30/04/2024.
//

#include "Walls.h"

#include <algorithm>
#include <App.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <array>
#include <glm/trigonometric.hpp>
#include <utils/ShaderManager.h>

#include "renderables/Renderable.h"
#include "utils/BoundingBox.h"


Walls::Walls() {
    shader = ShaderManager::GetInstance().get("BoundingBox");

    Wall forward;
    Wall backward;
    Wall left;
    Wall right;

    forward.position = {0.0F, 0.0F, 100.0F};
    forward.normal = {0.0F, 0.0F, -1.0F};
    forward.box = BoundingBox(glm::vec3(-100.0F, -100.0F, 100.0F), glm::vec3(100.0F, 100.0F, 100.0F));

    backward.position = {0.0F, 0.0F, -100.0F};
    backward.normal = {0.0F, 0.0F, 1.0F};
    backward.box = BoundingBox(glm::vec3(-100.0F, -100.0F, -100.0F), glm::vec3(100.0F, 100.0F, -100.0F));

    left.position = {-100.0F, 0.0F, 0.0F};
    left.normal = {1.0F, 0.0F, 0.0F};
    left.box = BoundingBox(glm::vec3(-100.0F, -100.0F, -100.0F), glm::vec3(-100.0F, 100.0F, 100.0F));

    right.position = {100.0F, 0.0F, 0.0F};
    right.normal = {-1.0F, 0.0F, 0.0F};
    right.box = BoundingBox(glm::vec3(100.0F, -100.0F, -100.0F), glm::vec3(100.0F, 100.0F, 100.0F));

    walls = {forward, backward, left, right};
}

void Walls::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    std::ranges::for_each(walls, [&](const auto &wall) {
        if (App::debug) {
            wall.box.draw(view, projection);
        }
    });
}

[[nodiscard]] auto Walls::getWalls() const -> std::array<Wall, 4> {
    return walls;
}
