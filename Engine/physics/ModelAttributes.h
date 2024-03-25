#ifndef PHYSICSATTRIBUTES_H
#define PHYSICSATTRIBUTES_H

#include "Config.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Physics {
    struct Attributes {
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 acceleration;
        glm::vec3 force;

        glm::vec3 rotation;
        glm::vec3 angularVelocity;
        glm::vec3 angularAcceleration;
        glm::vec3 torque;

        glm::mat4 transform;
        float radius;
        float mass;
        float damping;
        bool isGrounded = false;

        bool gravityAffected = true;

        Attributes()
                : position(0.0F), velocity(0.0F), acceleration(0.0F), force(0.0F),
                  mass(1.0F), damping(0.99F), radius(1.0F),
                  transform(Config::IDENTITY_MATRIX), rotation(0.0F), torque(0.0F), angularVelocity(0.0F),
                  angularAcceleration(0.0F) {}

        void update(float dt);

        void applyForce(const glm::vec3 &f);

        void applyGravity();

        void applyFriction(float friction);

        void applyDrag(float drag);

        void applySpring(const glm::vec3 &springAnchor, float springConstant,
                         float springLength);

        void applyImpulse(const glm::vec3 &impulse);

        [[nodiscard]] auto calculateForce(const glm::vec3 &point) const -> glm::vec3;

        [[nodiscard]] auto calculateTorque(const glm::vec3 &point) -> glm::vec3;

        void applyRotation(const glm::vec3 &rotation);

        void applyTorque(const glm::vec3 &torque);

        glm::vec3 calculateRotation(const glm::vec3 &point);
    };
} // namespace Physics

#endif // PHYSICSATTRIBUTES_H
