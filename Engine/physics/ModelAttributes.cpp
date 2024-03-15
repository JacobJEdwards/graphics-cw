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

void Physics::calculateCollisionResponse(Physics::Attributes &a,
                                         Physics::Attributes &b) {
  glm::vec3 normal = glm::normalize(b.position - a.position);
  glm::vec3 relativeVelocity = b.velocity - a.velocity;
  float relativeVelocityAlongNormal = glm::dot(relativeVelocity, normal);

  if (relativeVelocityAlongNormal > 0) {
    return;
  }

  float e = 0.5f;
  float j = -(1 + e) * relativeVelocityAlongNormal;
  j /= 1 / a.mass + 1 / b.mass;

  glm::vec3 impulse = j * normal;

  if (a.mass != 0.0F) {
    a.applyImpulse(-impulse);
  }
  if (b.mass != 0.0F) {
    b.applyImpulse(impulse);
  }
}

void Physics::calculateCollisionResponseFloor(Physics::Attributes &a,
                                              Physics::Attributes &b) {
  glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 relativeVelocity = a.velocity;
  float relativeVelocityAlongNormal = glm::dot(relativeVelocity, normal);

  if (relativeVelocityAlongNormal > 0) {
    return;
  }

  float e = 0.5f;
  float j = -(1 + e) * relativeVelocityAlongNormal;
  j /= 1 / a.mass;

  glm::vec3 impulse = j * normal;

  if (a.mass != 0.0F) {
    a.applyImpulse(impulse);
  }
}
