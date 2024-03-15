#include <glm/glm.hpp>

namespace Physics {
struct Attributes {
  glm::vec3 position;
  glm::vec3 velocity;
  glm::vec3 acceleration;
  glm::vec3 force;
  float mass;
  float damping;

  Attributes()
      : position(0.0f), velocity(0.0f), acceleration(0.0f), force(0.0f),
        mass(1.0f), damping(0.99f) {}

  void update(float dt);

  void applyForce(const glm::vec3 &f);

  void applyGravity();

  void applyFriction(float friction);

  void applyDrag(float drag);

  void applySpring(const glm::vec3 &springAnchor, float springConstant,
                   float springLength);

  void applyImpulse(const glm::vec3 &impulse);
};

} // namespace Physics
