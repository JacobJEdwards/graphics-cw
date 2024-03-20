#ifndef COLLISION_H
#define COLLISION_H

#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"
#include <glm/glm.hpp>


namespace Physics::Collisions {
    void resolve(Attributes &a, const glm::vec3 &normal);

    void resolve(Attributes &a, Attributes &b, const glm::vec3 &point);

    auto check(const BoundingBox &a, const BoundingBox &b) -> bool;

    auto check(const BoundingBox &a, const glm::vec3 &b) -> bool;

    auto getCollisionPoint(const BoundingBox &a, const BoundingBox &b) -> glm::vec3;
} // namespace Physics::Collisions


#endif
