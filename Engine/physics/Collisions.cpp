#include "Collisions.h"
#include "renderables/Entity.h"
#include "physics/Constants.h"
#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"
#include "renderables/objects/ProceduralTerrain.h"
#include <algorithm>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace {
    void resolveWithFloor(Physics::Attributes &a, const float floorY) {
        if (a.position.y < floorY) {
            a.position.y = floorY;
            a.velocity.y = -a.velocity.y;
        }
    }
} // namespace


namespace Physics::Collisions {
    void resolve(Attributes &a, const glm::vec3 &normal) {
        const glm::vec3 relativeVelocity = a.velocity;
        const float relativeVelocityAlongNormal = glm::dot(relativeVelocity, normal);

        if (relativeVelocityAlongNormal > 0.0F) {
            a.applyFriction(Physics::FRICTION);
            return;
        }

        if (a.mass == 0.0F) {
            a.velocity = glm::vec3(0.0F);
            return;
        }

        constexpr float e = 0.2F;
        float j = -(1 + e) * relativeVelocityAlongNormal;
        j /= 1 / a.mass;

        const glm::vec3 impulse = j * normal;

        const auto currentFront = a.getFront();
        const auto currentRight = a.getRight();
        const auto currentUp = a.getUp();

        const auto newFront = glm::normalize(glm::cross(currentRight, normal));
        const auto newRight = glm::normalize(glm::cross(normal, currentFront));

        const glm::quat currentOrientation = glm::quat_cast(glm::mat3(currentRight, currentUp, currentFront));

        const glm::quat targetOrientation = glm::quat_cast(glm::mat3(newRight, normal, newFront));

        a.currentOrientation = currentOrientation;
        a.targetOrientation = targetOrientation;

        const glm::quat interpolatedOrientation = glm::slerp(currentOrientation, targetOrientation, 0.1F);

        const glm::mat4 interpolatedRotationMatrix = glm::mat4_cast(interpolatedOrientation);

        a.transform[0] = glm::vec4(interpolatedRotationMatrix[0] * a.scale.x);
        a.transform[1] = glm::vec4(interpolatedRotationMatrix[1] * a.scale.y);
        a.transform[2] = glm::vec4(interpolatedRotationMatrix[2] * a.scale.z);
        a.transform[3] = glm::vec4(a.position, 1.0F);

        a.applyImpulse(impulse);
    }

    void resolve(Entity &a, const glm::vec3 &normal) {
        resolve(a.getAttributes(), normal);
    }

    void resolve(Attributes &a, Attributes &b) {
        if (a.mass == 0.0F && b.mass == 0.0F) {
            return;
        }

        const glm::vec3 normal = glm::normalize(b.position - a.position);
        const glm::vec3 relativeVelocity = b.velocity - a.velocity;
        const float relativeVelocityAlongNormal = glm::dot(relativeVelocity, normal);

        if (relativeVelocityAlongNormal > 0) {
            return;
        }

        constexpr float e = 0.5F;
        float j = -(1 + e) * relativeVelocityAlongNormal;

        if (a.mass != 0.0F && b.mass != 0.0F) {
            j /= 1 / a.mass + 1 / b.mass;
        } else if (a.mass == 0.0F) {
            j /= 1 / b.mass;
        } else if (b.mass == 0.0F) {
            j /= 1 / a.mass;
        }

        const glm::vec3 impulse = j * normal;


        if (a.mass != 0.0F) {
            a.applyImpulse(-impulse);
        }

        if (b.mass != 0.0F) {
            b.applyImpulse(impulse);
        }

        a.isColliding = true;
        b.isColliding = true;
    }

    void resolve(Attributes &a, Attributes &b, const glm::vec3 &point) {
        if (a.mass == 0.0F && b.mass == 0.0F) {
            return;
        }

        const glm::vec3 normal = glm::normalize(b.position - a.position);
        const glm::vec3 relativeVelocity = b.velocity - a.velocity;
        const float relativeVelocityAlongNormal = glm::dot(relativeVelocity, normal);

        if (relativeVelocityAlongNormal > 0) {
            return;
        }

        constexpr float e = 1.0F;
        float j = -(1 + e) * relativeVelocityAlongNormal;

        if (a.mass != 0.0F && b.mass != 0.0F) {
            j /= 1 / a.mass + 1 / b.mass;
        } else if (a.mass == 0.0F) {
            j /= 1 / b.mass;
        } else if (b.mass == 0.0F) {
            j /= 1 / a.mass;
        }

        const glm::vec3 impulse = j * normal;


        glm::vec3 rotationA = a.calculateRotation(point);
        glm::vec3 rotationB = b.calculateRotation(point);

        rotationA = glm::vec3(0.0F, rotationA.y, 0.0F);
        rotationB = glm::vec3(0.0F, rotationB.y, 0.0F);

        a.applyRotation(rotationA);
        b.applyRotation(rotationB);

        // apply the impulse
        if (a.mass != 0.0F) {
            a.applyImpulse(-impulse);
        }

        if (b.mass != 0.0F) {
            b.applyImpulse(impulse);
        }

        a.isColliding = true;
        b.isColliding = true;
    }

    void resolve(Entity &a, Entity &b, const glm::vec3 &collisionPoint) {
        resolve(a.getAttributes(), b.getAttributes(), collisionPoint);
    }

    void resolve(Entity &a, Entity &b) {
        const glm::vec3 collisionPoint = getCollisionPoint(a.getBoundingBox(), b.getBoundingBox());
        resolve(a.getAttributes(), b.getAttributes(), collisionPoint);
    }

    void resolve(Entity &a, const ProceduralTerrain &b) {
        const auto normal = b.getTerrainNormal(a.attributes.position.x, a.attributes.position.z);
        resolve(a, normal);
    }

    auto check(const BoundingBox &a, const BoundingBox &b) -> bool {
        return a.isColliding(b);
    }

    auto check(const BoundingBox &a, const glm::vec3 &b) -> bool { return a.contains(b); }

    auto getCollisionPoint(const BoundingBox &a, const BoundingBox &b) -> glm::vec3 {
        return a.getCollisionPoint(b);
    }

    auto getCollisionPoint(const Entity &a, const Entity &b) -> glm::vec3 {
        return getCollisionPoint(a.getBoundingBox(), b.getBoundingBox());
    }

    auto check(const Entity &a, const Entity &b) -> bool {
        return check(a.getBoundingBox(), b.getBoundingBox());
    }

    auto check(const Entity &a, const ProceduralTerrain &b) -> bool {
        return std::ranges::any_of(a.getBoundingBox().getCorners(), [&b](const auto &corner) {
            return b.intersectRay(corner);
        });
    }
} // namespace Physics::Collisions

