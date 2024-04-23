//
// Created by Jacob Edwards on 25/03/2024.
//

#include "Spline.h"
#include <algorithm>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include "graphics/Vertex.h"
#include <memory>
#include <vector>
#include <stdexcept>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/spline.hpp>

#include <span>
#include "graphics/buffers/VertexBuffer.h"
#include "graphics/Shader.h"
#include "utils/ShaderManager.h"
#include <GL/glew.h>
#include <memory>

#include "utils/Random.h"

namespace {
    // floating point errors
    template<typename T>
    auto bezier(T p1, T p2, T p3, T p4, const float t) -> T {
        return (1.0F - t) * (1.0F - t) * (1.0F - t) * p1 + 3.0F * (1.0F - t) * (1.0F - t) * t * p2 + 3.0F * (1.0F - t) *
               t * t * p3 +
               t * t * t * p4;
    }

    template<typename T>
    auto linear(T p1, T p2, float t) -> T {
        return (1.0F - t) * p1 + t * p2;
    }
}


Physics::Spline::Spline(std::span<const glm::vec3> points, const Type type, const float speed) : points(points.begin(),
        points.end()),
    type(type),
    speed(speed),
    numPoints(points.size()) {
    if (numPoints < 4U) {
        throw std::runtime_error("Spline must have at least 4 points");
    }

    buffer = std::make_unique<VertexBuffer>();
    std::vector<Vertex::Data> vertices;
    vertices.reserve(numPoints + 1U);

    for (const auto &point: points) {
        vertices.emplace_back(point, glm::vec3(0.0F), glm::vec2(0.0F));
    }

    vertices.emplace_back(*points.begin(), glm::vec3(0.0F), glm::vec2(0.0F));


    std::vector<GLuint> indices;
    indices.reserve(numPoints + 1U);

    for (GLuint i = 0U; i < numPoints; i++) {
        indices.push_back(i);
    }

    indices.push_back(0U);

    buffer->fill(vertices, indices);
    shader = ShaderManager::GetInstance().get("Base");
}

[[nodiscard]] auto Physics::Spline::getPoint() const -> glm::vec3 {
    switch (type) {
        case Type::CATMULLROM:
            return glm::catmullRom(points[p0Index], points[p1Index], points[p2Index],
                                   points[p3Index], t);
        case Type::HERMITE:
            return glm::hermite(points[p0Index], points[p1Index], points[p2Index],
                                points[p3Index], t);
        case Type::CUBIC:
            return glm::cubic(points[p0Index], points[p1Index], points[p2Index],
                              points[p3Index], t);
        case Type::BEZIER:
            return bezier(points[p0Index], points[p1Index], points[p2Index],
                          points[p3Index], t);
        case Type::LINEAR:
            return linear(points[p0Index], points[p1Index], t);
        default:
            return glm::vec3(0.0F);
    }
}

[[nodiscard]] auto Physics::Spline::getPoints() const -> const std::vector<glm::vec3> & {
    return points;
}

void Physics::Spline::update(const float dt) {
    p0Index = p0Index % numPoints;
    p1Index = p1Index % numPoints;
    p2Index = p2Index % numPoints;
    p3Index = p3Index % numPoints;

    t += dt * speed;

    if (t > 1.0F) {
        t = 0.0F;
        p0Index = (p0Index + 1U) % numPoints;
        p1Index = (p1Index + 1U) % numPoints;
        p2Index = (p2Index + 1U) % numPoints;
        p3Index = (p3Index + 1U) % numPoints;
        laps++;
    }
}

auto Physics::Spline::getLaps() const -> std::size_t {
    return laps;
}

void Physics::Spline::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();
    shader->setUniform("model", glm::mat4(1.0F));
    buffer->bind();
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLint>(numPoints + 1U));
    buffer->unbind();
}

void Physics::Spline::setSpeed(const float speed) {
    this->speed = speed;
}

void Physics::Spline::invert() {
    std::ranges::reverse(points);
}

void Physics::Spline::randomise() {
    // shift all points index in a random direction
    const auto shift = Random::Int(0, static_cast<int>(numPoints) - 1);

    std::ranges::rotate(points, points.begin() + shift);
}
