//
// Created by Jacob Edwards on 07/03/2024.
//

#ifndef CW_COLOR_H
#define CW_COLOR_H

#include <concepts>
#include <type_traits>

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

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
    constexpr glm::vec3 ORANGE = glm::vec3(1.0F, 0.5F, 0.0F);
    constexpr glm::vec3 PURPLE = glm::vec3(0.5F, 0.0F, 1.0F);
    constexpr glm::vec3 PINK = glm::vec3(1.0F, 0.0F, 0.5F);
    constexpr glm::vec3 LIME = glm::vec3(0.5F, 1.0F, 0.0F);
    constexpr glm::vec3 TEAL = glm::vec3(0.0F, 1.0F, 0.5F);
    constexpr glm::vec3 AQUA = glm::vec3(0.0F, 0.5F, 1.0F);
    constexpr glm::vec3 LAVENDER = glm::vec3(0.5F, 0.0F, 1.0F);
    constexpr glm::vec3 BROWN = glm::vec3(0.6F, 0.3F, 0.0F);
    constexpr glm::vec3 MAROON = glm::vec3(0.5F, 0.0F, 0.0F);
    constexpr glm::vec3 OLIVE = glm::vec3(0.5F, 0.5F, 0.0F);
    constexpr glm::vec3 NAVY = glm::vec3(0.0F, 0.0F, 0.5F);
    constexpr glm::vec3 GRAY = glm::vec3(0.5F, 0.5F, 0.5F);
    constexpr glm::vec3 SILVER = glm::vec3(0.75F, 0.75F, 0.75F);
    constexpr glm::vec3 GOLD = glm::vec3(1.0F, 0.84F, 0.0F);
    constexpr glm::vec3 BRONZE = glm::vec3(0.8F, 0.5F, 0.2F);
    constexpr glm::vec3 COPPER = glm::vec3(0.72F, 0.45F, 0.2F);
    constexpr glm::vec3 PLATINUM = glm::vec3(0.9F, 0.9F, 0.9F);
    constexpr glm::vec3 TURQUOISE = glm::vec3(0.25F, 0.88F, 0.82F);
    constexpr glm::vec3 EMERALD = glm::vec3(0.31F, 0.78F, 0.47F);
    constexpr glm::vec3 SAPPHIRE = glm::vec3(0.16F, 0.27F, 0.8F);
    constexpr glm::vec3 RUBY = glm::vec3(0.88F, 0.07F, 0.37F);
    constexpr glm::vec3 AMETHYST = glm::vec3(0.6F, 0.4F, 0.8F);
    constexpr glm::vec3 PEARL = glm::vec3(0.94F, 0.88F, 0.8F);
    constexpr glm::vec3 ONYX = glm::vec3(0.06F, 0.06F, 0.06F);
    constexpr glm::vec3 OPAL = glm::vec3(0.92F, 0.88F, 0.88F);
    constexpr glm::vec3 TOPAZ = glm::vec3(0.67F, 0.84F, 0.9F);
    constexpr glm::vec3 GARNET = glm::vec3(0.58F, 0.2F, 0.09F);
    constexpr glm::vec3 PERIDOT = glm::vec3(0.9F, 0.9F, 0.0F);
    constexpr glm::vec3 CITRINE = glm::vec3(0.89F, 0.79F, 0.0F);
    constexpr glm::vec3 AQUAMARINE = glm::vec3(0.5F, 1.0F, 0.83F);
    constexpr glm::vec3 JADE = glm::vec3(0.0F, 0.66F, 0.42F);
    constexpr glm::vec3 AMBER = glm::vec3(1.0F, 0.75F, 0.0F);
    constexpr glm::vec3 CORAL = glm::vec3(1.0F, 0.5F, 0.31F);
    constexpr glm::vec3 TAN = glm::vec3(0.82F, 0.71F, 0.55F);
    constexpr glm::vec3 KHAKI = glm::vec3(0.76F, 0.69F, 0.57F);
    constexpr glm::vec3 MAUVE = glm::vec3(0.88F, 0.69F, 0.8F);
    constexpr glm::vec3 LILAC = glm::vec3(0.78F, 0.64F, 0.78F);
    constexpr glm::vec3 INDIGO = glm::vec3(0.29F, 0.0F, 0.51F);

    template<typename T>
    constexpr auto fromRGB(T r, T g, T b) -> glm::vec3 requires std::integral<T> || std::floating_point<T> {
        static_assert(std::is_arithmetic_v<T>, "Color::fromRGB requires an arithmetic type");
        return {r / RGB_MAX, g / RGB_MAX, b / RGB_MAX};
    }

    template<typename T>
    constexpr auto fromRGB(T r, T g, T b, T a) -> glm::vec4 requires std::integral<T> || std::floating_point<T> {
        static_assert(std::is_arithmetic_v<T>, "Color::fromRGB requires an arithmetic type");
        return {r / RGB_MAX, g / RGB_MAX, b / RGB_MAX, a / RGB_MAX};
    }
}

#endif // CW_COLOR_H
