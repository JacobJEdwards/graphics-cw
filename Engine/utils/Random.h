//
// Created by Jacob Edwards on 08/04/2024.
//

#ifndef CW_RANDOM_H
#define CW_RANDOM_H

#include <random>
#include <ctime>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Random {
    auto Float(float min, float max) -> float;

    auto Int(int min, int max) -> int;

    auto Vec3(float min, float max) -> glm::vec3;
}


#endif //CW_RANDOM_H
