//
// Created by Jacob Edwards on 11/03/2024.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <glm/glm.hpp>

namespace Config {
    constexpr auto DEFAULT_WIDTH = 1200;
    constexpr auto DEFAULT_HEIGHT = 900;
    constexpr auto DEFAULT_TITLE = "CW";
    constexpr auto DEFAULT_ASPECT = static_cast<float>(DEFAULT_WIDTH) / static_cast<float>(DEFAULT_HEIGHT);
    constexpr auto DEFAULT_FOV = 45.0F;
    constexpr auto DEFAULT_NEAR = 0.1F;
    constexpr auto DEFAULT_FAR = 1000.0F;

    constexpr auto IDENTITY_MATRIX = glm::mat4(1.0F);
}

#endif //CONFIG_H
