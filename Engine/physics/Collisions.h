#ifndef COLLISION_H
#define COLLISION_H

/*
 * https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection
 * https://isaacphysics.org/concepts/cp_collisions?stage=all
 */

#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"
#include "renderables/Entity.h"
#include "renderables/objects/ProceduralTerrain.h"
#include <glm/ext/vector_float3.hpp>


namespace Physics::Collisions {
    void resolve(Attributes &a, const glm::vec3 &normal);

    void resolve(Entity &a, const glm::vec3 &normal);

    void resolve(Attributes &a, Attributes &b, const glm::vec3 &point);

    void resolve(Attributes &a, Attributes &b);

    void resolve(Entity &a, Entity &b);

    void resolve(Entity &a, const ProceduralTerrain &b);

    void resolve(Entity &a, Entity &b, const glm::vec3 &collisionPoint);

    auto check(const BoundingBox &a, const BoundingBox &b) -> bool;

    auto check(const BoundingBox &a, const glm::vec3 &b) -> bool;

    auto check(const Entity &a, const Entity &b) -> bool;

    auto check(const Entity &a, const ProceduralTerrain &b) -> bool;

    auto getCollisionPoint(const BoundingBox &a, const BoundingBox &b) -> glm::vec3;

    auto getCollisionPoint(const Entity &a, const Entity &b) -> glm::vec3;
} // namespace Physics::Collisions


#endif
