#ifndef PHYSICSATTRIBUTES_H
#define PHYSICSATTRIBUTES_H

#include "Config.h"
#include "physics/Spline.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

  void update(float dt);

  void applyForce(const glm::vec3 &f);

  void applyGravity();

  void applyFriction(float friction);

  void applyDrag(float drag);

  void applySpring(const glm::vec3 &springAnchor, float springConstant,
                   float springLength);

  void applyImpulse(const glm::vec3 &impulse);

  auto calculateForce(const Spline &spline, float t, float dt) -> glm::vec3;
};
} // namespace Physics

#endif // PHYSICSATTRIBUTES_H
