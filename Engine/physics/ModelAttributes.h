#ifndef PHYSICSATTRIBUTES_H
#define PHYSICSATTRIBUTES_H

#include "Config.h"
#include <glm/glm.hpp>

namespace Physics {
struct Attributes {
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  glm::vec3 force;
  glm::mat4 transform;
  float radius;
  float mass;
  float damping;
  bool isGrounded = false;

  Attributes()
      : position(0.0f), velocity(0.0f), acceleration(0.0f), force(0.0f),
        mass(1.0f), damping(0.99f), radius(1.0F),
        transform(Config::IDENTITY_MATRIX) {}

  virtual void update(float dt);

  void applyForce(const glm::vec3 &f);

  void applyGravity();

  void applyFriction(float friction);

  void applyDrag(float drag);

  virtual void applySpring(const glm::vec3 &springAnchor, float springConstant,
                           float springLength);

  void applyImpulse(const glm::vec3 &impulse);
};

struct Pendelumn : public Attributes {
  float length;
  float angle;
  float angularVelocity;
  float angularAcceleration;
  glm::vec3 pivot;

  Pendelumn(float length, float angle, float angularVelocity,
            float angularAcceleration, const glm::vec3 &pivot)
      : length(length), angle(angle), angularVelocity(angularVelocity),
        angularAcceleration(angularAcceleration), pivot(pivot) {}

  void update(float dt) override {
    angularVelocity += angularAcceleration * dt;
    angle += angularVelocity * dt;

    position.x = pivot.x + length * sin(angle);
    position.y = pivot.y - length * cos(angle);
  }

  void applySpring(const glm::vec3 &springAnchor, float springConstant,
                   float springLength) override {
    glm::vec3 springVector = springAnchor - pivot;
    float currentLength = glm::length(springVector);

    glm::vec3 springForce = springConstant * (currentLength - springLength) *
                            glm::normalize(springVector);
    applyForce(springForce);
  }
};
} // namespace Physics

#endif // PHYSICSATTRIBUTES_H
