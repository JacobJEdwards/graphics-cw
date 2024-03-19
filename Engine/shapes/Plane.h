#ifndef PLANE_H
#define PLANE_H

#include "utils/Buffer.h"
#include "utils/Vertex.h"
#include <glm/glm.hpp>
#include <vector>

class Plane {
public:
  Plane() { buffer.fill(vertices); }

  void draw(GLuint texture) const {
    buffer.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    buffer.draw();
    buffer.unbind();
  }

private:
  Buffer buffer;
  std::vector<Vertex::Data> vertices{
      Vertex::Data{glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
      Vertex::Data{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
      Vertex::Data{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
      Vertex::Data{glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
      Vertex::Data{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
      Vertex::Data{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)}};
};

#endif
