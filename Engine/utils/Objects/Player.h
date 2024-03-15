//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "utils/BoundingBox.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "graphics/Model.h"
#include "utils/Shader.h"

class Player {
public:
  Player() { player.setShader(shader); };

  [[nodiscard]] auto getPosition() const -> glm::vec3 { return position; }

  void setPosition(const glm::vec3 &position) { Player::position = position; }

  // issue is that original reset the mat every time, this tranlsates it
  // could isntead calculate
  // or just reset the mat every time
  void draw(const glm::mat4 &view, const glm::mat4 &projection) {
    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);

    player.resetModelMatrix();

    player.translate(position);
    player.translate(glm::vec3(0.0F, -7.0F, 0.0F));
    // player.translate(glm::vec3(0.0F, 0.0F, -1.3F));

    player.draw();
  }

  [[nodiscard]] auto getBoundingBox() const -> BoundingBox {
    return player.getBoundingBox();
  }

  [[nodiscard]] auto isColliding(const BoundingBox &other) const -> bool {
    return player.getBoundingBox().isColliding(other);
  }

  void move(const glm::vec3 &direction) { position += direction; }

private:
  glm::vec3 position;

  std::shared_ptr<Shader> shader = std::make_shared<Shader>(
      "../Assets/shaders/base.vert", "../Assets/shaders/base.frag");

  Model player = Model("../Assets/objects/person/person.obj");
};

#endif // PLAYER_H
