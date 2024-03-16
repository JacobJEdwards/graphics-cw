#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <glm/glm.hpp>

namespace Physics {
constexpr float AIR_RESISTANCE = 0.1f;
constexpr float FRICTION = 0.1f;
constexpr glm::vec3 FLOOR_NORMAL = glm::vec3(0.0f, 1.0f, 0.0f);
} // namespace Physics

#endif
