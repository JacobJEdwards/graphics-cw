//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils/Buffer.h"

class BoundingBox {
public:
  BoundingBox() = default;

  BoundingBox(glm::vec3 min, glm::vec3 max);
  BoundingBox(const aiVector3D &min, const aiVector3D &max);

  // destructor
  ~BoundingBox() = default;

  // copy constructor
  BoundingBox(const BoundingBox &other) = default;

  [[nodiscard]] auto getMin() const -> glm::vec3;

  [[nodiscard]] auto getMax() const -> glm::vec3;

  void setMin(const glm::vec3 &min);

  void setMax(const glm::vec3 &max);

  void transform(const glm::mat4 &model);

  bool collides(const BoundingBox &other) const;

  bool contains(const glm::vec3 &point) const;

  bool contains(const BoundingBox &other) const;

  bool isColliding(const BoundingBox &other) const;

  void setPosition(const glm::vec3 &position);

  glm::vec3 getCenter() const;

  glm::vec3 getSize() const;

  void translate(const glm::vec3 &translation);

  void scale(const glm::vec3 &scale);

  void rotate(const glm::vec3 &axis, float angle);

  auto getOffset(const glm::vec3 &point) const -> glm::vec3;

  auto getOffset(const BoundingBox &other) const -> glm::vec3;

  void expand(const glm::vec3 &amount);

  void expand(const BoundingBox &other);

  void draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection) const;

private:
  glm::vec3 min;
  glm::vec3 max;

  Buffer buffer;

  void initBuffer();
};

#endif // BOUNDINGBOX_H
