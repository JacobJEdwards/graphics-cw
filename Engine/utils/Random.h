//
// Created by Jacob Edwards on 08/04/2024.
//

#ifndef CW_RANDOM_H
#define CW_RANDOM_H

#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <ranges>
#include <glm/vec2.hpp>

namespace Random {
    auto Float(float min, float max) -> float;

    auto Int(int min, int max) -> int;

    auto Vec2(float min, float max) -> glm::vec2;

    auto Vec3(float min, float max) -> glm::vec3;

    auto Vec4(float min, float max) -> glm::vec4;

    template<std::ranges::range R>
    auto Element(const R &range) -> typename R::value_type {
        return range[Random::Int(0, range.size() - 1)];
    }

    template<std::ranges::range R>
    auto Element(const R &range, const int min, const int max) -> typename R::value_type {
        return range[Int(min, max)];
    }
}


#endif //CW_RANDOM_H
