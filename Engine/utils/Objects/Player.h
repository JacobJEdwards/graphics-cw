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

#include "graphics/Model.h"
#include "utils/Camera.h"
#include "utils/Shader.h"

class Player {
public:
  Player();

  enum class Mode { FPS, FREE, ORBIT, FIXED, PATH };
  enum class Direction { FORWARD, BACKWARD, LEFT, RIGHT, NONE, UP, DOWN };

  void processKeyboard(Direction direction, float deltaTime);

  [[nodiscard]] auto getCamera() const -> Camera &;
  [[nodiscard]] auto getPosition() const -> glm::vec3;

  void update(float dt);

  void draw(const glm::mat4 &view, const glm::mat4 &projection,
            bool show = true);

  [[nodiscard]] auto getBoundingBox() const -> BoundingBox;

  void jump();

  void setDrawModel(bool draw);

  auto getAttributes() -> Physics::Attributes &;

  void setMode(Mode mode);

  void interface();

  void debug() const;

  void nitro();

private:
  Mode mode = Mode::FPS;

  bool drawModel = true;

  float jumpForce = 100.0F;

  std::unique_ptr<Camera> camera =
      std::make_unique<Camera>(glm::vec3(0.0F, 5.0F, 3.0F));

  std::shared_ptr<Shader> shader = std::make_shared<Shader>(
      "../Assets/shaders/base.vert", "../Assets/shaders/base.frag");

  Model model = Model("../Assets/objects/person/person.obj");
};

#endif // PLAYER_H
