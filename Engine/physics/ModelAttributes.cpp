#include "physics/ModelAttributes.h"

#include <algorithm>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/trigonometric.hpp>

#include "Config.h"
#include "physics/Constants.h"
#include "physics/Gravity.h"

// constexpr float ZERO_THRESHHOLD = 0.00000000000000000001;

void Physics::Attributes::update(const float dt) {
    previousTransform = transform;


    const glm::quat interpolatedOrientation = glm::slerp(currentOrientation, targetOrientation, 0.1F);

    const glm::mat4 interpolatedRotationMatrix = glm::mat4_cast(interpolatedOrientation);

    const auto scale = glm::vec3(glm::length(transform[0]), glm::length(transform[1]),
                                 glm::length(transform[2]));

    transform[0] = glm::vec4(interpolatedRotationMatrix[0] * scale.x);
    transform[1] = glm::vec4(interpolatedRotationMatrix[1] * scale.y);
    transform[2] = glm::vec4(interpolatedRotationMatrix[2] * scale.z);
    transform[3] = glm::vec4(position, 1.0F);


    velocity += acceleration * dt;
    position += velocity * dt;
    acceleration = force / mass;

    angularVelocity += angularAcceleration * dt;
    rotation += angularVelocity * dt;
    angularAcceleration = torque / mass;

    glm::mat4 rotationMatrix = Config::IDENTITY_MATRIX;

    constexpr float rotationDamping = 1.0F - Physics::ANGULAR_DAMPING;

    rotation *= rotationDamping;

    rotationMatrix = glm::rotate(rotationMatrix, rotation.x * dt, glm::vec3(1.0F, 0.0F, 0.0F));
    rotationMatrix = glm::rotate(rotationMatrix, rotation.y * dt, glm::vec3(0.0F, 1.0F, 0.0F));
    rotationMatrix = glm::rotate(rotationMatrix, rotation.z * dt, glm::vec3(0.0F, 0.0F, 1.0F));

    transform = transform * rotationMatrix;

    transform[3][0] = position.x;
    transform[3][1] = position.y;
    transform[3][2] = position.z;

    force = glm::vec3(0.0F);
    torque = glm::vec3(0.0F);

    applyDrag(Physics::AIR_RESISTANCE * glm::length(velocity));

    if (isGrounded) {
        applyFriction(Physics::FRICTION);
    } else {
        if (gravityAffected) {
            applyGravity();
        }
    }
}

void Physics::Attributes::applyForce(const glm::vec3 &f) { force += f; }

void Physics::Attributes::applyGravity() {
    if (!gravityAffected) {
        return;
    }

    applyForce(Physics::GRAVITY_VECTOR * mass);
}

void Physics::Attributes::applyFriction(const float friction) {
    applyForce(-velocity * friction);
    applyTorque(-angularVelocity * friction);
}

void Physics::Attributes::applyDrag(const float drag) {
    applyForce(-velocity * drag);
    applyTorque(-angularVelocity * drag);
}

void Physics::Attributes::applyImpulse(const glm::vec3 &impulse) {
    if (mass == 0.0F) {
        return;
    }

    velocity += impulse / mass;
}

void Physics::Attributes::applySpring(const glm::vec3 &springAnchor,
                                      const float springConstant,
                                      const float springLength) {
    const glm::vec3 springVector = springAnchor - position;
    const glm::vec3 springForce = springConstant *
                                  (glm::length(springVector) - springLength) *
                                  glm::normalize(springVector);
    applyForce(springForce);
}

auto Physics::Attributes::calculateForce(const glm::vec3 &point) const
    -> glm::vec3 {
    if (mass == 0.0F) {
        return glm::vec3(0.0F);
    }

    if (point == position) {
        return glm::vec3(0.0F);
    }

    const glm::vec3 initialVelocity = velocity;

    const glm::vec3 finalVelocity = point - position;

    const glm::vec3 acceleration = finalVelocity - initialVelocity;

    const glm::vec3 force = mass * acceleration;

    return force;
}

auto Physics::Attributes::calculateRotation(const glm::vec3 &normal, const glm::vec3 &axis) const -> glm::vec3 {
    const glm::vec3 direction = glm::normalize(normal - glm::normalize(position));
    const glm::vec3 forward = glm::normalize(glm::vec3(transform[1]));

    auto rotation = glm::vec3(0.0F);

    if (direction != forward) {
        const float angle = glm::acos(glm::dot(forward, direction));

        const glm::quat rotationQuat = glm::angleAxis(angle, axis);

        rotation = glm::eulerAngles(rotationQuat);
    }

    return rotation;
}

auto Physics::Attributes::calculateRotation(const glm::vec3 &point) const -> glm::vec3 {
    const glm::vec3 direction = glm::normalize(point - position);
    const glm::vec3 forward = getFront();

    auto rotation = glm::vec3(0.0F);

    if (direction != forward) {
        const glm::vec3 axis = glm::cross(forward, direction);
        const float angle = glm::acos(glm::dot(forward, direction));

        const glm::quat rotationQuat = glm::angleAxis(angle, axis);

        rotation = glm::eulerAngles(rotationQuat);
    }

    return rotation;
}


void Physics::Attributes::applyRotation(const glm::vec3 &rotation) {
    this->rotation += rotation;
}

void Physics::Attributes::applyPitch(const float angle) {
    rotation.x = angle;
}

void Physics::Attributes::applyRoll(const float angle) {
    rotation.z = angle;
}

void Physics::Attributes::applyYaw(const float angle) {
    rotation.y += angle;
}

void Physics::Attributes::applyTorque(const glm::vec3 &torque) {
    this->torque += torque;
}

auto Physics::Attributes::getTransform() const -> glm::mat4 {
    return transform;
}

auto Physics::Attributes::getPreviousTransform() const -> glm::mat4 {
    return previousTransform;
}

auto Physics::Attributes::getFront() const -> glm::vec3 {
    return glm::normalize(glm::vec3(transform[2]));
}

auto Physics::Attributes::getUp() const -> glm::vec3 {
    return glm::normalize(glm::vec3(transform[1]));
}

auto Physics::Attributes::getRight() const -> glm::vec3 {
    return glm::normalize(glm::vec3(transform[0]));
}

auto Physics::Attributes::getPosition() const -> glm::vec3 {
    return position;
}
