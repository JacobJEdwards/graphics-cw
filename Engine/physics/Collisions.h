#ifndef COLLISION_H
#define COLLISION_H

#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"
#include <glm/glm.hpp>

namespace Physics {
namespace Collisions {
void resolve(Attributes &a, const glm::vec3 &normal);
void resolve(Attributes &a, Attributes &b);
bool check(const BoundingBox &a, const BoundingBox &b);
bool check(const BoundingBox &a, const glm::vec3 &b);
} // namespace Collisions
} // namespace Physics

#endif
