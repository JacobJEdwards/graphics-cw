#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <glm/glm.hpp>

namespace Physics {
    constexpr float AIR_RESISTANCE = 0.1F;
    constexpr float FRICTION = 0.3F;
    constexpr glm::vec3 FLOOR_NORMAL = glm::vec3(0.0F, 1.0F, 0.0F);
    constexpr float ROTATION_THRESHOLD = 0.04F;
    constexpr float ANGULAR_DAMPING = 0.05F;
} // namespace Physics

#endif
