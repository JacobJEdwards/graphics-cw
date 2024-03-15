//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "helpers/AssimpGLMHelpers.h"
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class BoundingBox {
public:
  BoundingBox() = default;

  BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {}
  BoundingBox(const aiVector3D &min, const aiVector3D &max)
      : min(AssimpGLMHelpers::getGLMVec(min)),
        max(AssimpGLMHelpers::getGLMVec(max)) {}

  [[nodiscard]] auto getMin() const -> glm::vec3 { return min; }

  [[nodiscard]] auto getMax() const -> glm::vec3 { return max; }

  void setMin(const glm::vec3 &min) { BoundingBox::min = min; }

  void setMax(const glm::vec3 &max) { BoundingBox::max = max; }

  void transform(const glm::mat4 &model) {
    glm::vec3 newMin = glm::vec3(model * glm::vec4(min, 1.0F));
    glm::vec3 newMax = glm::vec3(model * glm::vec4(max, 1.0F));

    min = glm::min(newMin, newMax);
    max = glm::max(newMin, newMax);
  }

  bool collides(const BoundingBox &other) const {
    for (int i = 0; i < 3; ++i) {
      if (max[i] < other.min[i] || min[i] > other.max[i]) {
        // No overlap on this axis, so no collision
        return false;
      }
    }
    return true;
  }

  bool contains(const glm::vec3 &point) const {
    return point.x >= min.x && point.x <= max.x && point.y >= min.y &&
           point.y <= max.y && point.z >= min.z && point.z <= max.z;
  }

  bool contains(const BoundingBox &other) const {
    return contains(other.min) && contains(other.max);
  }

  bool isColliding(const BoundingBox &other) const {
    return collides(other) || contains(other) || other.contains(*this);
  }

  void setPosition(const glm::vec3 &position) {
    auto center = getCenter();
    auto offset = position - center;

    translate(offset);
  }

  glm::vec3 getCenter() const { return (min + max) / 2.0F; }

  glm::vec3 getSize() const { return max - min; }

  void translate(const glm::vec3 &translation) {
    min += translation;
    max += translation;
  }

  void scale(const glm::vec3 &scale) {
    min *= scale;
    max *= scale;
  }

  void rotate(const glm::vec3 &axis, float angle) {
    auto center = getCenter();
    auto rotationMatrix = glm::rotate(glm::mat4(1.0F), angle, axis);
    min = glm::vec3(rotationMatrix * glm::vec4(min - center, 1.0F)) + center;
    max = glm::vec3(rotationMatrix * glm::vec4(max - center, 1.0F)) + center;
  }

  auto getOffset(const glm::vec3 &point) const -> glm::vec3 {
    if (!contains(point)) {
      return glm::vec3(0.0F);
    }

    float distanceToXMin = std::abs(point.x - min.x);
    float distanceToXMax = std::abs(point.x - max.x);
    float distanceToYMin = std::abs(point.y - min.y);
    float distanceToYMax = std::abs(point.y - max.y);
    float distanceToZMin = std::abs(point.z - min.z);
    float distanceToZMax = std::abs(point.z - max.z);

    if (distanceToXMin < distanceToXMax && distanceToXMin < distanceToYMin &&
        distanceToXMin < distanceToYMax && distanceToXMin < distanceToZMin &&
        distanceToXMin < distanceToZMax) {
      return glm::vec3(-distanceToXMin, 0.0F, 0.0F);
    }

    if (distanceToXMax < distanceToXMin && distanceToXMax < distanceToYMin &&
        distanceToXMax < distanceToYMax && distanceToXMax < distanceToZMin &&
        distanceToXMax < distanceToZMax) {
      return glm::vec3(distanceToXMax, 0.0F, 0.0F);
    }

    if (distanceToYMin < distanceToXMin && distanceToYMin < distanceToXMax &&
        distanceToYMin < distanceToYMax && distanceToYMin < distanceToZMin &&
        distanceToYMin < distanceToZMax) {
      return glm::vec3(0.0F, -distanceToYMin, 0.0F);
    }

    if (distanceToYMax < distanceToXMin && distanceToYMax < distanceToXMax &&
        distanceToYMax < distanceToYMin && distanceToYMax < distanceToZMin &&
        distanceToYMax < distanceToZMax) {
      return glm::vec3(0.0F, distanceToYMax, 0.0F);
    }

    if (distanceToZMin < distanceToXMin && distanceToZMin < distanceToXMax &&
        distanceToZMin < distanceToYMin && distanceToZMin < distanceToYMax &&
        distanceToZMin < distanceToZMax) {
      return glm::vec3(0.0F, 0.0F, -distanceToZMin);
    }

    if (distanceToZMax < distanceToXMin && distanceToZMax < distanceToXMax &&
        distanceToZMax < distanceToYMin && distanceToZMax < distanceToYMax &&
        distanceToZMax < distanceToZMin) {
      return glm::vec3(0.0F, 0.0F, distanceToZMax);
    }

    return glm::vec3(0.0F);
  }

  auto getOffset(const BoundingBox &other) const -> glm::vec3 {
    glm::vec3 overlap = glm::min(max, other.max) - glm::max(min, other.min);

    if (overlap.x < 0 || overlap.y < 0 || overlap.z < 0) {
      return glm::vec3(0.0f);
    }

    float minOverlap = std::min(std::min(overlap.x, overlap.y), overlap.z);

    if (overlap.x == minOverlap) {
      return glm::vec3(overlap.x, 0.0f, 0.0f);
    } else if (overlap.y == minOverlap) {
      return glm::vec3(0.0f, overlap.y, 0.0f);
    } else {
      return glm::vec3(0.0f, 0.0f, overlap.z);
    }
  }

  void expand(const glm::vec3 &amount) {
    min -= amount;
    max += amount;
  }

  void expand(const BoundingBox &other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
  }

private:
  glm::vec3 min;
  glm::vec3 max;
};

#endif // BOUNDINGBOX_H
