//
// Created by Jacob Edwards on 27/02/2024.
//

#ifndef SUN_H
#define SUN_H

#include "Config.h"
#include "graphics/Model.h"
#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "utils/Shader.h"

class Sun {
public:
  Sun() { sun.setShader(shader); };

  void update(const float dt) {
    angle += 0.5F * dt;
    position.x = 10.0F * std::cos(angle);
    position.y = 10.0F * std::sin(angle);
  }

  void draw(const glm::mat4 &view, const glm::mat4 &projection) {
    glDepthFunc(GL_LEQUAL);

    const auto newView = glm::mat4(glm::mat3(view));

    shader->use();
    shader->setUniform("view", newView);
    shader->setUniform("projection", projection);

    auto model = Config::IDENTITY_MATRIX;
    model = glm::translate(model, glm::vec3(position, -10.0F));
    model = glm::scale(model, glm::vec3(scale));

    sun.setModelMatrix(model);

    sun.draw();

    glDepthFunc(GL_LESS);
  }

  void setPosition(const glm::vec2 &pos) { position = pos; }

  void setScale(const float newScale) { scale = newScale; }

  [[nodiscard]] auto getPosition() const -> glm::vec2 { return position; }

private:
  std::shared_ptr<Shader> shader = std::make_shared<Shader>(
      "../Assets/shaders/sun.vert", "../Assets/shaders/sun.frag");
  Model sun = Model("../Assets/objects/sun/sun.obj");

  glm::vec2 position = glm::vec2(0.0F, 0.0F);
  float scale = 0.001F;
  float angle = 0.0F;
};

#endif // SUN_H
