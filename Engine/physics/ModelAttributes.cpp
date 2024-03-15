#include "physics/ModelAttributes.h"

#include <glm/glm.hpp>

#include "physics/Gravity.h"

void Physics::Attributes::update(float dt) {
  velocity += acceleration * dt;
  position += velocity * dt;
  acceleration = force / mass;
  force = glm::vec3(0.0f);

  velocity *= damping;
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
  velocity += impulse / mass;
}

void Physics::Attributes::applySpring(const glm::vec3 &springAnchor,
                                      float springConstant,
                                      float springLength) {
  glm::vec3 springVector = springAnchor - position;
  glm::vec3 springForce = springConstant *
                          (glm::length(springVector) - springLength) *
                          glm::normalize(springVector);
  applyForce(springForce);
}
