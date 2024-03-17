#include "Collisions.h"
#include "physics/Constants.h"
#include "physics/ModelAttributes.h"

namespace {
void resolveWithFloor(Physics::Attributes &a, float floorY) {
  if (a.position.y < floorY) {
    a.position.y = floorY;
    a.velocity.y = -a.velocity.y;
  }
}
} // namespace

namespace Physics {
namespace Collisions {
void resolve(Attributes &a, const glm::vec3 &normal) {
  glm::vec3 relativeVelocity = a.velocity;
  float relativeVelocityAlongNormal = glm::dot(relativeVelocity, normal);

  if (relativeVelocityAlongNormal > 0) {
    a.applyFriction(Physics::FRICTION);
    return;
  }

  if (a.mass == 0.0F) {
    a.velocity = glm::vec3(0.0F);
    return;
  }

  float e = 0.5f;
  float j = -(1 + e) * relativeVelocityAlongNormal;
  j /= 1 / a.mass;

  glm::vec3 impulse = j * normal;

  a.applyImpulse(impulse);
}
void resolve(Attributes &a, Attributes &b) {
  // edge case
  if (a.mass == 0.0F && b.mass == 0.0F) {
    return;
  }

  glm::vec3 normal = glm::normalize(b.position - a.position);
  glm::vec3 relativeVelocity = b.velocity - a.velocity;
  float relativeVelocityAlongNormal = glm::dot(relativeVelocity, normal);

  if (relativeVelocityAlongNormal > 0) {
    return;
  }

  // eventually calculate this
  // energe conservation -> how bouncy it is
  float e = 0.5f;
  float j = -(1 + e) * relativeVelocityAlongNormal;

  // clean this up
  if (a.mass != 0.0F && b.mass != 0.0F) {
    j /= 1 / a.mass + 1 / b.mass;
  } else if (a.mass == 0.0F) {
    j /= 1 / b.mass;
  } else if (b.mass == 0.0F) {
    j /= 1 / a.mass;
  }

  glm::vec3 impulse = j * normal;

  if (a.mass != 0.0F) {
    a.applyImpulse(-impulse);
  }

  if (b.mass != 0.0F) {
    b.applyImpulse(impulse);
  }
}

bool check(const BoundingBox &a, const BoundingBox &b) {
  return a.isColliding(b);
}

bool check(const BoundingBox &a, const glm::vec3 &b) { return a.contains(b); }
} // namespace Collisions
} // namespace Physics
