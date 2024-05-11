//
// Created by Jacob Edwards on 30/04/2024.
//

#ifndef WALLS_H
#define WALLS_H

#include <algorithm>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <array>
#include <glm/trigonometric.hpp>
#include <graphics/Model.h>
#include <utils/ShaderManager.h>

#include "renderables/Renderable.h"
#include "utils/BoundingBox.h"


class Walls final : public Renderable {
public:
    using Renderable::draw;

    struct Wall {
        BoundingBox box;
        glm::vec3 position;
        glm::vec3 normal;
    };

    Walls();

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    [[nodiscard]] auto getWalls() const -> std::array<Wall, 4>;

private:
    std::array<Wall, 4> walls;
};


#endif //WALLS_H
