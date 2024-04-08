//
// Created by Jacob Edwards on 08/04/2024.
//

#include "Random.h"

//
// Created by Jacob Edwards on 08/04/2024.
//

#include <glm/ext/vector_float3.hpp>
#include <random>

namespace Random {
    std::random_device rnd;
    std::mt19937 gen(rnd());

    auto Float(float min, float max) -> float {
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    auto Int(int min, int max) -> int {
        std::uniform_int_distribution<int> dis(min, max);
        return dis(gen);
    }

    auto Vec3(float min, float max) -> glm::vec3 {
        return {Float(min, max), Float(min, max), Float(min, max)};
    }

    auto Vec4(float min, float max) -> glm::vec4 {
        return {Float(min, max), Float(min, max), Float(min, max), Float(min, max)};
    }
}
