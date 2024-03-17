//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "Config.h"
#include "graphics/Model.h"
#include "utils/Camera.h"
#include "utils/Shader.h"

class Player {
public:
  Physics::Attributes attributes;

  Player() {
    boundingBox = model.getBoundingBox();
    model.setShader(shader);
    camera->setMode(Camera::Mode::FPS);
    camera->setPosition(glm::vec3(0.0F, 20.0F, 3.0F));
    position = camera->getPosition();
  };

  [[nodiscard]] auto getCamera() const -> Camera & { return *camera.get(); }

  [[nodiscard]] auto getPosition() const -> glm::vec3 { return position; }

  void update(float dt) {
    position = camera->getPosition();
    model.update(dt);
    camera->update(dt);
    attributes.position = position;
    attributes.update(dt);
  }

  void draw(const glm::mat4 &view, const glm::mat4 &projection,
            bool show = true) {

    boundingBox.setPosition(position);

    auto modelMat = Config::IDENTITY_MATRIX;
    modelMat = glm::translate(modelMat, position);
    modelMat = glm::translate(modelMat, glm::vec3(0.0F, -6.0F, 0.0F));

    model.setModelMatrix(modelMat);

    if (show && drawModel) {
      shader->use();
      shader->setUniform("view", view);
      shader->setUniform("projection", projection);
      // boundingBox.transform(modelMat);
      model.draw();
    }
  }

  [[nodiscard]] auto getBoundingBox() const -> BoundingBox {
    return boundingBox;
  }

  [[nodiscard]] auto isColliding(const BoundingBox &other) const -> bool {
    return boundingBox.isColliding(other);
  }

  void jump() { camera->jump(); }

  void setDrawModel(bool draw) { drawModel = draw; }

private:
  bool drawModel = true;
  glm::vec3 position;
  BoundingBox boundingBox =
      BoundingBox(glm::vec3(-0.5F, -0.5F, -0.5F), glm::vec3(0.5F, 0.5F, 0.5F));

  std::unique_ptr<Camera> camera =
      std::make_unique<Camera>(glm::vec3(0.0F, 5.0F, 3.0F));

  std::shared_ptr<Shader> shader = std::make_shared<Shader>(
      "../Assets/shaders/base.vert", "../Assets/shaders/base.frag");

  Model model = Model("../Assets/objects/person/person.obj");
};

#endif // PLAYER_H
