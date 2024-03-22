#include "physics/ModelAttributes.h"

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

#include "physics/Constants.h"
#include "physics/Gravity.h"

void Physics::Attributes::update(float dt) {
    velocity += acceleration * dt;
    position += velocity * dt;
    acceleration = force / mass;

    angularVelocity += angularAcceleration * dt;
    rotation += angularVelocity * dt;
    angularAcceleration = torque / mass;

    // clean this up proper !!!!!!!!!
    if (glm::length(rotation) > 0.0F) {
        float xRotation = 0.0F;
        // float xRotation = rotation.x < 0 ? -0.02F : 0.02F;
        float yRotation = rotation.y < 0 ? -0.02F : 0.02F;
        float zRotation = 0.0F;
        // float zRotation = rotation.z < 0 ? -0.02F : 0.02F;

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0F), xRotation, glm::vec3(1.0F, 0.0F, 0.0F));
        rotationMatrix = glm::rotate(rotationMatrix, yRotation, glm::vec3(0.0F, 1.0F, 0.0F));
        rotationMatrix = glm::rotate(rotationMatrix, zRotation, glm::vec3(0.0F, 0.0F, 1.0F));

        transform = rotationMatrix * transform;

        if (xRotation < 0.0F) {
            rotation.x <= 0.0F ? rotation.x = 0.0F : rotation.x -= xRotation;
        } else {
            rotation.x >= 0.0F ? rotation.x = 0.0F : rotation.x -= xRotation;

        }

        if (yRotation < 0.0F) {
            rotation.y <= 0.0F ? rotation.y = 0.0F : rotation.y -= yRotation;
        } else {
            rotation.y >= 0.0F ? rotation.y = 0.0F : rotation.y -= yRotation;
        }

        if (zRotation < 0.0F) {
            rotation.z <= 0.0F ? rotation.z = 0.0F : rotation.z -= zRotation;
        } else {
            rotation.z >= 0.0F ? rotation.z = 0.0F : rotation.z -= zRotation;
        }
    }


    // applyRotation(rotation);

    // rotation = glm::vec3(0.0F);

    transform[3][0] = position.x;
    transform[3][1] = position.y;
    transform[3][2] = position.z;

    force = glm::vec3(0.0F);
    torque = glm::vec3(0.0F);

    applyDrag(Physics::AIR_RESISTANCE * glm::length(velocity));

    if (isGrounded) {
        applyFriction(Physics::FRICTION);
    }
}

void Physics::Attributes::applyForce(const glm::vec3 &f) { force += f; }

void Physics::Attributes::applyGravity() {
    applyForce(Physics::GRAVITY_VECTOR * mass);
}

void Physics::Attributes::applyFriction(float friction) {
    applyForce(-velocity * friction);
}

void Physics::Attributes::applyDrag(float drag) {
    applyForce(-velocity * drag);
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

auto Physics::Attributes::calculateForce(const glm::vec3 &point, float dt) const
-> glm::vec3 {
    if (dt == 0.0F) {
        return glm::vec3(0.0F);
    }

    if (mass == 0.0F) {
        return glm::vec3(0.0F);
    }

    if (point == position) {
        return glm::vec3(0.0F);
    }

    glm::vec3 initialVelocity = velocity;

    glm::vec3 finalVelocity = (point - position) / dt;

    glm::vec3 acceleration = (finalVelocity - initialVelocity) / dt;

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

    glm::vec3 torque = glm::vec3(0.0f);

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
