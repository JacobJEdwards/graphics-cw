//
// Created by Jacob Edwards on 25/03/2024.
//

#include "Spline.h"
#include <vector>
#include <vector>
#include <stdexcept>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/spline.hpp>

#include <glm/glm.hpp>
#include <span>

namespace {
    // floating point errors
    template<typename T>
    auto bezier(T p1, T p2, T p3, T p4, float t) -> T {
        return (1 - t) * (1 - t) * (1 - t) * p1 + 3 * (1 - t) * (1 - t) * t * p2 + 3 * (1 - t) * t * t * p3 +
               t * t * t * p4;
    }

    template<typename T>
    auto linear(T p1, T p2, float t) -> T {
        return (1 - t) * p1 + t * p2;
    }
}


Physics::Spline::Spline(std::span<const glm::vec3> points, Type type, float speed) : points(points.begin(),
                                                                                            points.end()),
                                                                                     type(type),
                                                                                     speed(speed),
                                                                                     numPoints(points.size()) {
    if (numPoints < 4) {
        throw std::runtime_error("Spline must have at least 4 points");
    }
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
        p0Index = (p0Index + 1) % numPoints;
        p1Index = (p1Index + 1) % numPoints;
        p2Index = (p2Index + 1) % numPoints;
        p3Index = (p3Index + 1) % numPoints;
    }
}

void Physics::Spline::setSpeed(const float speed) {
    this->speed = speed;
}

void Physics::Spline::invert() {
    std::reverse(points.begin(), points.end());
}