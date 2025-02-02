//
// Created by Jacob Edwards on 11/03/2024.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace Config {
    constexpr auto DEFAULT_WIDTH = 1200;
    constexpr auto DEFAULT_HEIGHT = 900;
    constexpr auto DEFAULT_TITLE = "CW";
    constexpr auto DEFAULT_ASPECT =
            static_cast<float>(DEFAULT_WIDTH) / static_cast<float>(DEFAULT_HEIGHT);
    constexpr auto IDENTITY_MATRIX = glm::mat4(1.0F);
    constexpr auto ZERO_VECTOR = glm::vec3(0.0F);
} // namespace Config

#endif // CONFIG_H
