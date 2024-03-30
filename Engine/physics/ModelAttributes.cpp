#include "physics/ModelAttributes.h"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

#include "Config.h"
#include "physics/Constants.h"
#include "physics/Gravity.h"

void Physics::Attributes::update(float dt) {
    velocity += acceleration * dt;
    position += velocity * dt;
    acceleration = force / mass;

    angularVelocity += angularAcceleration * dt;
    rotation += angularVelocity * dt;
    angularAcceleration = torque / mass;

    glm::mat4 rotationMatrix = Config::IDENTITY_MATRIX;

    const float rotationDamping = 1.0F - Physics::ANGULAR_DAMPING;
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

void Physics::Attributes::applyFriction(float friction) {
    applyForce(-velocity * friction);
    applyTorque(-angularVelocity * friction);
}

void Physics::Attributes::applyDrag(float drag) {
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
                                      float springConstant,
                                      float springLength) {
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

    glm::vec3 initialVelocity = velocity;

    glm::vec3 finalVelocity = (point - position);;

    glm::vec3 acceleration = (finalVelocity - initialVelocity);

    glm::vec3 force = mass * acceleration;

    return force;
}

auto Physics::Attributes::calculateRotation(const glm::vec3 &point) -> glm::vec3 {
    glm::vec3 direction = glm::normalize(point - position);
    glm::vec3 forward = glm::normalize(glm::vec3(transform[2]));

    auto rotation = glm::vec3(0.0F);

    if (direction != forward) {
        glm::vec3 axis = glm::cross(forward, direction);
        float angle = glm::acos(glm::dot(forward, direction));

        rotation = axis * angle;
    }

    return rotation;
}

// calculate minimum rotation needed to rotate the object to face the point
auto Physics::Attributes::calculateTorque(const glm::vec3 &point) -> glm::vec3 {
    glm::vec3 direction = glm::normalize(point - position);
    glm::vec3 forward = glm::normalize(glm::vec3(transform[2]));

    auto torque = glm::vec3(0.0F);

    if (direction != forward) {
        glm::vec3 axis = glm::cross(forward, direction);
        float angle = glm::acos(glm::dot(forward, direction));
        torque = axis * angle;
    }

    return torque;
}

void Physics::Attributes::applyRotation(const glm::vec3 &rotation) {
    this->rotation += rotation;
}

void Physics::Attributes::applyTorque(const glm::vec3 &torque) {
    this->torque += torque;
}
