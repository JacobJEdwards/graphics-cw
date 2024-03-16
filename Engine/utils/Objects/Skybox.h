//
// Created by Jacob Edwards on 24/02/2024.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <array>
#include <glm/ext/matrix_float4x4.hpp>
#include <span>
#include <string>

#include "graphics/Texture.h"
#include "utils/Buffer.h"
#include "utils/Shader.h"

constexpr unsigned int NUM_VERTEX = 36;
constexpr unsigned int NUM_FACES = 6;

// TODO
// REFACTOR THIS
class Skybox {
public:
  explicit Skybox(std::span<const std::string, NUM_FACES> faces) {
    init();
    loadTextures(faces);
  }

  void draw(const glm::mat4 &projection, const glm::mat4 &view,
            const GLfloat y) const {
    glDepthFunc(GL_LEQUAL);
    const auto newView = glm::mat4(glm::mat3(view));
    shader.use();

    shader.setUniform("view", newView);
    shader.setUniform("projection", projection);
    shader.setUniform("intensity", y);

    buffer.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
    buffer.draw();
    buffer.unbind();
    glDepthFunc(GL_LESS);
  }

private:
  Texture::Data texture;
  Buffer buffer;

  const std::array<Vertex::Data, 36> vertices = {
      Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},
      Vertex::Data{glm::vec3(-1.0F, -1.0F, -1.0F)},
      Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
      Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
      Vertex::Data{glm::vec3(1.0F, 1.0F, -1.0F)},
      Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},

      Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},
      Vertex::Data{glm::vec3(-1.0F, -1.0F, -1.0F)},
      Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},
      Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},
      Vertex::Data{glm::vec3(-1.0F, 1.0F, 1.0F)},
      Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},

      Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
      Vertex::Data{glm::vec3(1.0F, -1.0F, 1.0F)},
      Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
      Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
      Vertex::Data{glm::vec3(1.0F, 1.0F, -1.0F)},
      Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},

      Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},
      Vertex::Data{glm::vec3(-1.0F, 1.0F, 1.0F)},
      Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
      Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
      Vertex::Data{glm::vec3(1.0F, -1.0F, 1.0F)},
      Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},

      Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},
      Vertex::Data{glm::vec3(1.0F, 1.0F, -1.0F)},
      Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
      Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
      Vertex::Data{glm::vec3(-1.0F, 1.0F, 1.0F)},
      Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},

      Vertex::Data{glm::vec3(-1.0F, -1.0F, -1.0F)},
      Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},
      Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
      Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
      Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},
      Vertex::Data{glm::vec3(1.0F, -1.0F, 1.0F)}};

  const std::array<GLuint, 36> indices = {0,  1,  2,  3,  4,  5,

                                          6,  7,  8,  9,  10, 11,

                                          12, 13, 14, 15, 16, 17,

                                          18, 19, 20, 21, 22, 23,

                                          24, 25, 26, 27, 28, 29,

                                          30, 31, 32, 33, 34, 35};

  Shader shader =
      Shader("../Assets/shaders/skybox.vert", "../Assets/shaders/skybox.frag");

  void init() {
    buffer.fill(vertices, indices);

    shader.use();
    shader.setUniform("skybox", 0);
  }

  void loadTextures(std::span<const std::string, NUM_FACES> faces) {
    texture.id = Texture::Loader::loadCubemap(faces);
    ;
    texture.type = Texture::Type::CUBEMAP;
  }
};

#endif // SKYBOX_H
