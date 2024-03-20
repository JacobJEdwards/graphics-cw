//
// Created by Jacob Edwards on 13/03/2024.
//

#include "Player.h"

#include "imgui/imgui.h"
#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "graphics/Model.h"
#include "utils/Camera.h"

Player::Player() { model.setShader(shader); };

void Player::processKeyboard(const Direction direction, const float deltaTime) {
  if (camera->getMode() == Camera::Mode::ORBIT) {
    return;
  }

  auto front = camera->getFront();
  auto right = camera->getRight();
  auto up = camera->getUp();

  switch (direction) {
  case Direction::FORWARD:
    model.attributes.applyForce(front * 10.0F);
    break;
  case Direction::BACKWARD:
    model.attributes.applyForce(-front * 10.0F);
    break;
  case Direction::LEFT:
    model.attributes.applyForce(-right * 10.0F);
    break;
  case Direction::RIGHT:
    model.attributes.applyForce(right * 10.0F);
    break;
  case Direction::NONE:
    model.attributes.applyDrag(0.5F);
    break;
  case Direction::UP:
    model.attributes.applyForce(up * 10.0F);
    break;
  case Direction::DOWN:
    model.attributes.applyForce(-up * 10.0F);
    break;
  }
}

[[nodiscard]] auto Player::getCamera() const -> Camera & {
  return *camera.get();
}

[[nodiscard]] auto Player::getPosition() const -> glm::vec3 {
  return model.attributes.position;
}

void Player::update(float dt) {
  bool gravity = mode == Mode::FPS;
  model.update(dt, gravity);
  camera->setPosition(model.attributes.position + glm::vec3(0.0F, 4.0F, 0.0F));
  camera->update(dt);
}

void Player::draw(const glm::mat4 &view, const glm::mat4 &projection,
                  bool show) {

  if (show && drawModel) {
    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    // boundingBox.transform(modelMat);
    model.draw(view, projection);
  }
}

[[nodiscard]] auto Player::getBoundingBox() const -> BoundingBox {
  return model.getBoundingBox();
}

void Player::setDrawModel(bool draw) { drawModel = draw; }

auto Player::getAttributes() -> Physics::Attributes & {
  return model.attributes;
}

void Player::setMode(Mode mode) {
  Player::mode = mode;
  switch (mode) {
  case Mode::FPS:
    camera->setMode(Camera::Mode::FPS);
    break;
  case Mode::ORBIT:
    camera->setMode(Camera::Mode::ORBIT);
    break;
  case Mode::FREE:
    camera->setMode(Camera::Mode::FREE);
    break;
  case Mode::FIXED:
    camera->setMode(Camera::Mode::FIXED);
    break;
  case Mode::PATH:
    camera->setMode(Camera::Mode::PATH);
    break;
  }
}

void Player::jump() {
  if (model.attributes.isGrounded) {
    model.attributes.applyForce(glm::vec3(0.0F, jumpForce, 0.0F));
  }
}

void Player::debug() const {
  ImGui::Begin("Player Debug");
  ImGui::Text("Position: (%.2f, %.2f, %.2f)", model.attributes.position.x,
              model.attributes.position.y, model.attributes.position.z);
  ImGui::Text("Velocity: (%.2f, %.2f, %.2f)", model.attributes.velocity.x,
              model.attributes.velocity.y, model.attributes.velocity.z);
  ImGui::Text("Acceleration: (%.2f, %.2f, %.2f)",
              model.attributes.acceleration.x, model.attributes.acceleration.y,
              model.attributes.acceleration.z);
  ImGui::Text("Force: (%.2f, %.2f, %.2f)", model.attributes.force.x,
              model.attributes.force.y, model.attributes.force.z);
  ImGui::End();
}

void Player::interface() {
  ImGui::Begin("Player");
  ImGui::SliderFloat("Jump Force", &jumpForce, 0.0F, 10000.0F);
  ImGui::SliderFloat("Mass", &model.attributes.mass, 0.1F, 1000.0F);
  ImGui::End();
}

void Player::nitro() {
  model.attributes.applyForce(camera->getFront() * 200.0F);
}
