//
// Created by Jacob Edwards on 07/03/2024.
//

#ifndef CW_COLOR_H
#define CW_COLOR_H

#include <concepts>

#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_float3.hpp>


namespace Color {
    constexpr float RGB_MAX = 255.0F;

    constexpr glm::vec3 WHITE = glm::vec3(1.0F, 1.0F, 1.0F);
    constexpr glm::vec3 BLACK = glm::vec3(0.0F, 0.0F, 0.0F);
    constexpr glm::vec3 RED = glm::vec3(1.0F, 0.0F, 0.0F);
    constexpr glm::vec3 GREEN = glm::vec3(0.0F, 1.0F, 0.0F);
    constexpr glm::vec3 BLUE = glm::vec3(0.0F, 0.0F, 1.0F);
    constexpr glm::vec3 YELLOW = glm::vec3(1.0F, 1.0F, 0.0F);
    constexpr glm::vec3 CYAN = glm::vec3(0.0F, 1.0F, 1.0F);
    constexpr glm::vec3 MAGENTA = glm::vec3(1.0F, 0.0F, 1.0F);

    template <typename T>
    constexpr auto fromRGB(T r, T g, T b) -> glm::vec3 requires std::integral<T> || std::floating_point<T> {
        static_assert(std::is_arithmetic_v<T>, "Color::fromRGB requires an arithmetic type");
        return glm::vec3(r / RGB_MAX, g / RGB_MAX, b / RGB_MAX);
    }

    template <typename T>
    constexpr auto fromRGB(T r, T g, T b, T a) -> glm::vec4 requires std::integral<T> || std::floating_point<T> {
        static_assert(std::is_arithmetic_v<T>, "Color::fromRGB requires an arithmetic type");
        return glm::vec4(r / RGB_MAX, g / RGB_MAX, b / RGB_MAX, a / RGB_MAX);
    }
}


#endif //CW_COLOR_H
