#ifndef PHYSICSATTRIBUTES_H
#define PHYSICSATTRIBUTES_H

#include <glm/fwd.hpp>

#include "Config.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Physics {
    struct Attributes {
        glm::vec3 position = Config::ZERO_VECTOR;
        glm::vec3 velocity = Config::ZERO_VECTOR;
        glm::vec3 acceleration = Config::ZERO_VECTOR;
        glm::vec3 force = Config::ZERO_VECTOR;

        glm::vec3 rotation = Config::ZERO_VECTOR;
        glm::vec3 angularVelocity = Config::ZERO_VECTOR;
        glm::vec3 angularAcceleration = Config::ZERO_VECTOR;
        glm::vec3 torque = Config::ZERO_VECTOR;

        glm::mat4 transform = Config::IDENTITY_MATRIX;
        glm::mat4 previousTransform = Config::IDENTITY_MATRIX;

        glm::mat4 targetRotation = Config::IDENTITY_MATRIX;

        glm::quat currentOrientation = glm::quat_cast(transform);
        glm::quat targetOrientation = glm::quat_cast(targetRotation);

        glm::vec3 scale = glm::vec3(1.0F);

        float radius = 1.0F;
        float mass = 1.0F;
        float damping = 0.99F;
        bool isGrounded = false;
        bool isColliding = false;

        bool gravityAffected = true;

        Attributes() = default;

        void update(float dt);

        void applyForce(const glm::vec3 &f);

        void applyGravity();

        void applyFriction(float friction);

        void applyDrag(float drag);

        void applySpring(const glm::vec3 &springAnchor, float springConstant,
                         float springLength);

        void applyImpulse(const glm::vec3 &impulse);

        [[nodiscard]] auto calculateForce(const glm::vec3 &point) const -> glm::vec3;

        void applyRotation(const glm::vec3 &rotation);

        void applyPitch(float angle);

        void applyYaw(float angle);

        void applyRoll(float angle);

        void applyTorque(const glm::vec3 &torque);

        auto calculateRotation(const glm::vec3 &point) const -> glm::vec3;

        auto calculateRotation(const glm::vec3 &normal, const glm::vec3 &axis) const -> glm::vec3;

        [[nodiscard]] auto getTransform() const -> glm::mat4;

        [[nodiscard]] auto getPreviousTransform() const -> glm::mat4;

        [[nodiscard]] auto getFront() const -> glm::vec3;

        [[nodiscard]] auto getUp() const -> glm::vec3;

        [[nodiscard]] auto getRight() const -> glm::vec3;

        [[nodiscard]] auto getPosition() const -> glm::vec3;
    };
} // namespace Physics

#endif // PHYSICSATTRIBUTES_H
