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

    constexpr auto WHITE = glm::vec3(1.0F, 1.0F, 1.0F);
    constexpr auto BLACK = glm::vec3(0.0F, 0.0F, 0.0F);
    constexpr auto RED = glm::vec3(1.0F, 0.0F, 0.0F);
    constexpr auto GREEN = glm::vec3(0.0F, 1.0F, 0.0F);
    constexpr auto BLUE = glm::vec3(0.0F, 0.0F, 1.0F);
    constexpr auto YELLOW = glm::vec3(1.0F, 1.0F, 0.0F);
    constexpr auto CYAN = glm::vec3(0.0F, 1.0F, 1.0F);
    constexpr auto MAGENTA = glm::vec3(1.0F, 0.0F, 1.0F);
    constexpr auto ORANGE = glm::vec3(1.0F, 0.5F, 0.0F);
    constexpr auto PURPLE = glm::vec3(0.5F, 0.0F, 1.0F);
    constexpr auto PINK = glm::vec3(1.0F, 0.0F, 0.5F);
    constexpr auto LIME = glm::vec3(0.5F, 1.0F, 0.0F);
    constexpr auto TEAL = glm::vec3(0.0F, 1.0F, 0.5F);
    constexpr auto AQUA = glm::vec3(0.0F, 0.5F, 1.0F);
    constexpr auto LAVENDER = glm::vec3(0.5F, 0.0F, 1.0F);
    constexpr auto BROWN = glm::vec3(0.6F, 0.3F, 0.0F);
    constexpr auto MAROON = glm::vec3(0.5F, 0.0F, 0.0F);
    constexpr auto OLIVE = glm::vec3(0.5F, 0.5F, 0.0F);
    constexpr auto NAVY = glm::vec3(0.0F, 0.0F, 0.5F);
    constexpr auto GRAY = glm::vec3(0.5F, 0.5F, 0.5F);
    constexpr auto SILVER = glm::vec3(0.75F, 0.75F, 0.75F);
    constexpr auto GOLD = glm::vec3(1.0F, 0.84F, 0.0F);
    constexpr auto BRONZE = glm::vec3(0.8F, 0.5F, 0.2F);
    constexpr auto COPPER = glm::vec3(0.72F, 0.45F, 0.2F);
    constexpr auto PLATINUM = glm::vec3(0.9F, 0.9F, 0.9F);
    constexpr auto TURQUOISE = glm::vec3(0.25F, 0.88F, 0.82F);
    constexpr auto EMERALD = glm::vec3(0.31F, 0.78F, 0.47F);
    constexpr auto SAPPHIRE = glm::vec3(0.16F, 0.27F, 0.8F);
    constexpr auto RUBY = glm::vec3(0.88F, 0.07F, 0.37F);
    constexpr auto AMETHYST = glm::vec3(0.6F, 0.4F, 0.8F);
    constexpr auto PEARL = glm::vec3(0.94F, 0.88F, 0.8F);
    constexpr auto ONYX = glm::vec3(0.06F, 0.06F, 0.06F);
    constexpr auto OPAL = glm::vec3(0.92F, 0.88F, 0.88F);
    constexpr auto TOPAZ = glm::vec3(0.67F, 0.84F, 0.9F);
    constexpr auto GARNET = glm::vec3(0.58F, 0.2F, 0.09F);
    constexpr auto PERIDOT = glm::vec3(0.9F, 0.9F, 0.0F);
    constexpr auto CITRINE = glm::vec3(0.89F, 0.79F, 0.0F);
    constexpr auto AQUAMARINE = glm::vec3(0.5F, 1.0F, 0.83F);
    constexpr auto JADE = glm::vec3(0.0F, 0.66F, 0.42F);
    constexpr auto AMBER = glm::vec3(1.0F, 0.75F, 0.0F);
    constexpr auto CORAL = glm::vec3(1.0F, 0.5F, 0.31F);
    constexpr auto TAN = glm::vec3(0.82F, 0.71F, 0.55F);
    constexpr auto KHAKI = glm::vec3(0.76F, 0.69F, 0.57F);
    constexpr auto MAUVE = glm::vec3(0.88F, 0.69F, 0.8F);
    constexpr auto LILAC = glm::vec3(0.78F, 0.64F, 0.78F);
    constexpr auto INDIGO = glm::vec3(0.29F, 0.0F, 0.51F);
    constexpr auto DARKGREY = glm::vec3(0.2, 0.2, 0.2);

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
