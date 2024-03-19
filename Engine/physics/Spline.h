#ifndef SPLINE_H
#define SPLINE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <utility>
#include <vector>

namespace Physics {
class Spline {
public:
  Spline(std::vector<glm::vec3> points, float tension = 0.5f)
      : points(std::move(points)), tension(tension) {}
  auto getPoint(float t) const -> glm::vec3 {
    int i = static_cast<int>(t);
    t -= i;
    return interpolate(t, i);
  }

  auto getTangent(float t) const -> glm::vec3 {
    int i = static_cast<int>(t);
    t -= i;
    glm::vec3 p0 = i > 0 ? points[i - 1] : points[i];
    glm::vec3 p1 = points[i];
    glm::vec3 p2 = i < points.size() - 1 ? points[i + 1] : points[i];
    glm::vec3 p3 = i < points.size() - 2 ? points[i + 2] : points[i];
    return 0.5f * ((-p0 + p2) + (4.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t +
                   (-3.0f * p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t);
  }
  auto getNormal(float t) const -> glm::vec3 {
    glm::vec3 tangent = getTangent(t);
    return glm::normalize(glm::cross(tangent, glm::vec3(0, 1, 0)));
  }

  auto getBinormal(float t) const -> glm::vec3 {
    glm::vec3 tangent = getTangent(t);
    glm::vec3 normal = getNormal(t);
    return glm::cross(tangent, normal);
  }

private:
  std::vector<glm::vec3> points;
  float tension;

  auto interpolate(float t, int i) const -> glm::vec3 {
    glm::vec3 p0 = i > 0 ? points[i - 1] : points[i];
    glm::vec3 p1 = points[i];
    glm::vec3 p2 = i < points.size() - 1 ? points[i + 1] : points[i];
    glm::vec3 p3 = i < points.size() - 2 ? points[i + 2] : points[i];
    return 0.5f * ((2.0f * p1) + (-p0 + p2) * t +
                   (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t +
                   (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t);
  }
};
} // namespace Physics

#endif
