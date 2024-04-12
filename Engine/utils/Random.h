//
// Created by Jacob Edwards on 08/04/2024.
//

#ifndef CW_RANDOM_H
#define CW_RANDOM_H

#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <ctime>

namespace Random {
    auto Float(float min, float max) -> float;

    auto Int(int min, int max) -> int;

    auto Vec3(float min, float max) -> glm::vec3;

    auto Vec4(float min, float max) -> glm::vec4;
}


#endif //CW_RANDOM_H
