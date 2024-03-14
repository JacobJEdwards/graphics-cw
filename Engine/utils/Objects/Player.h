//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "utils/BoundingBox.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Config.h"
#include "graphics/Model.h"
#include "utils/Shader.h"

class Player {
public:
  Player() { boundingBox = player.getBoundingBox(); };

  [[nodiscard]] auto getPosition() const -> glm::vec3 { return position; }

  void setPosition(const glm::vec3 &position) {
    Player::position = position;
    boundingBox.setPosition(position);
  }

  void draw(const glm::mat4 &view, const glm::mat4 &projection) {
    shader.use();
    shader.setUniform("view", view);
    shader.setUniform("projection", projection);
    auto model = Config::IDENTITY_MATRIX;
    model = glm::translate(model, position);
    model = glm::translate(model, glm::vec3(0.0F, -7.0F, 0.0F));
    // translate back a bit
    model = glm::translate(model, glm::vec3(0.0F, 0.0F, -1.3F));

    // boundingBox.transform(model);

    shader.setUniform("model", model);
    player.draw(&shader);
  }

  [[nodiscard]] auto getBoundingBox() const -> BoundingBox {
    return boundingBox;
  }

  [[nodiscard]] auto isColliding(const BoundingBox &other) const -> bool {
    return boundingBox.isColliding(other);
  }

  void move(const glm::vec3 &direction) {
    position += direction;
    boundingBox.setPosition(position);
  }

private:
  glm::vec3 position;

  BoundingBox boundingBox =
      BoundingBox(glm::vec3(-0.5F, -0.5F, -0.5F), glm::vec3(0.5F, 0.5F, 0.5F));

  Shader shader =
      Shader("../Assets/shaders/base.vert", "../Assets/shaders/base.frag");
  Model player = Model("../Assets/objects/person/person.obj");
};

#endif // PLAYER_H
