#include "physics/ModelAttributes.h"

#include <glm/glm.hpp>

#include "physics/Constants.h"
#include "physics/Gravity.h"

void Physics::Attributes::update(float dt) {
  velocity += acceleration * dt;
  position += velocity * dt;
  acceleration = force / mass;

  transform[3][0] = position.x;
  transform[3][1] = position.y;
  transform[3][2] = position.z;

  force = glm::vec3(0.0F);

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

auto Physics::Attributes::calculateForce(const glm::vec3 &point, float dt)
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

  // Calculate initial velocity
  glm::vec3 initialVelocity = velocity;

  // Calculate final velocity
  glm::vec3 finalVelocity = (point - position) / dt;

  // Calculate acceleration required
  glm::vec3 acceleration = (finalVelocity - initialVelocity) / dt;

  // Calculate force
  glm::vec3 force = mass * acceleration;

  return force;
}
