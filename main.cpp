#include <exception>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>

#include <glm/ext/matrix_transform.hpp>

#include "App.h"
#include "utils/Camera.h"

#include "graphics/Color.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"
#include "utils/Objects/InfinitePlane.h"
#include "utils/Objects/Player.h"
#include "utils/Objects/Skybox.h"
#include "utils/Objects/Sun.h"
#include "utils/Shader.h"

bool useMouse = false;

void processInput();

auto main() -> int {
  App::window("Coursework", App::DEFAULT_WIDTH, App::DEFAULT_HEIGHT);
  App::init();

  App::view.setKey(processInput);
  App::view.setMouse([&] {
    if (!App::paused) {
      App::camera.processMouseMovement(App::view.getMouseOffsetX(),
                                       App::view.getMouseOffsetY());
    }
  });

  App::view.setScroll(
      [&] { App::camera.processMouseScroll(App::view.getScrollY()); });

  App::view.setResize([&] {
    App::camera.setAspect(static_cast<float>(App::view.getWidth()) /
                          App::view.getHeight());
    glViewport(0, 0, static_cast<GLsizei>(App::view.getWidth()),
               static_cast<GLsizei>(App::view.getHeight()));
  });

  App::view.setMenu([&] {
    glfwSetInputMode(App::view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    View::clearTarget(Color::BLACK);
  });

  App::view.setShowMenu(false);

  std::shared_ptr<Shader> ourShader = std::make_shared<Shader>(
      "../assets/shaders/backpack.vert", "../Assets/shaders/backpack.frag");

  const std::array<std::string, 6> skyboxFaces{
      "../Assets/textures/skybox/right.jpg",
      "../Assets/textures/skybox/left.jpg",
      "../Assets/textures/skybox/top.jpg",
      "../Assets/textures/skybox/bottom.jpg",
      "../Assets/textures/skybox/front.jpg",
      "../Assets/textures/skybox/back.jpg",
  };

  const Skybox skybox(skyboxFaces);

  Texture::Loader::setFlip(false);
  Model newModel("../Assets/objects/helecopter/chopper.obj");
  Texture::Loader::setFlip(true);
  Model model2("../Assets/objects/backpack/backpack.obj");
  Texture::Loader::setFlip(false);

  newModel.setShader(ourShader);
  model2.setShader(ourShader);

  Sun sun;
  Player person;
  InfinitePlane terrain;

  App::camera.setOrbit(glm::vec3(0.0F, 0.0F, 0.0F), 10.0F, 5.0F, 5.0F);
  App::camera.setMode(Camera::Mode::ORBIT);

  ourShader->use();
  ourShader->setUniform("projection", App::camera.getProjectionMatrix());
  ourShader->setUniform("light.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
  ourShader->setUniform("light.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
  ourShader->setUniform("light.specular", glm::vec3(1.0F, 1.0F, 1.0F));
  // material properties
  ourShader->setUniform("material.specular", glm::vec3(0.5F, 0.5F, 0.5F));
  ourShader->setUniform("material.shininess", 64.0F);

  glm::vec3 newPosition =
      App::camera.getPosition() + glm::vec3(0.0F, 4.0F, 0.45F);

  newModel.translate(newPosition);
  newModel.scale(glm::vec3(0.1F, 0.1F, 0.1F));

  newPosition = App::camera.getPosition() + glm::vec3(5.0F, -0.15F, 0.45F);

  model2.translate(newPosition);

  sun.setPosition(App::camera.getPosition());

  App::view.setPipeline([&]() {
    const auto projectionMatrix = App::camera.getProjectionMatrix();

    View::clearTarget(Color::BLACK);

    ourShader->use();

    ourShader->setUniform("projection", projectionMatrix);

    ourShader->setUniform("light.position", glm::vec3(sun.getPosition(), 1.0F));
    // view/projection transformations
    glm::mat4 view = App::camera.getViewMatrix();
    ourShader->setUniform("view", view);
    ourShader->setUniform("viewPos", App::camera.getPosition());
    newModel.draw();

    if (newModel.isColliding(person.getBoundingBox())) {
      std::cout << "Colliding" << std::endl;
      const auto offset = newModel.getOffset(person.getBoundingBox());
      App::camera.setPosition(App::camera.getPosition() + offset);
      auto velocity = App::camera.getVelocity();
      if (std::abs(offset.y) > 0.0F) {
        velocity.y = 0.0F;
      }

      if (std::abs(offset.x) > 0.0F) {
        velocity.x = 0.0F;
      }

      if (std::abs(offset.z) > 0.0F) {
        velocity.z = 0.0F;
      }

      App::camera.setVelocity(velocity);
    }

    model2.draw();

    if (model2.isColliding(person.getBoundingBox())) {
      const auto offset = model2.getOffset(person.getBoundingBox());
      App::camera.setPosition(App::camera.getPosition() + offset);
      auto velocity = App::camera.getVelocity();
      if (std::abs(offset.y) > 0.0F) {
        velocity.y = 0.0F;
      }

      if (std::abs(offset.x) > 0.0F) {
        velocity.x = 0.0F;
      }

      if (std::abs(offset.z) > 0.0F) {
        velocity.z = 0.0F;
      }

      App::camera.setVelocity(velocity);
    }

    terrain.draw(view, projectionMatrix, glm::vec3(sun.getPosition(), 1.0F),
                 App::camera.getPosition());

    if (terrain.isColliding(person.getBoundingBox())) {
      const auto offset = terrain.getOffset(person.getBoundingBox());
      if (std::abs(offset.y) > 0.0F) {
        App::camera.setPosition(App::camera.getPosition() + offset);
        auto velocity = App::camera.getVelocity();
        velocity.y = 0.0F;
      }
    }

    view = glm::mat4(glm::mat3(App::camera.getViewMatrix()));
    skybox.draw(projectionMatrix, view, sun.getPosition().y);
    sun.draw(view, projectionMatrix);

    person.draw(App::camera.getViewMatrix(), projectionMatrix);
  });

  App::view.setInterface([&]() {
    if (App::paused) {
      App::camera.modeInterface();
      App::camera.controlInterface();
    }
  });

  try {
    App::loop([&] {
      sun.update(App::view.getDeltaTime());
      App::camera.circleOrbit(App::view.getDeltaTime());
      person.setPosition(App::camera.getPosition());
    });
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  App::quit();
}

// move keys keys into function
// could store array of key presses, flip to 1 if pressed 0 otherwise
// then do update stuff based on that
// might be better in a callback or something
// prototype:

/** def input(glEnum(?) key, std::function<void()> callback, keyAction =
 * GLFW_PRESS); */

// might be cleaner, however need to change moved thing, unless keep App::camera
// controls separate if in class namespace or something can keep window as
// global save on fuzz
void processInput() {
  bool moved = false;

  if (App::view.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    App::view.close();
  }

  if (App::view.getKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
    App::setPaused(!App::paused);
  }

  if (App::paused && App::camera.getMode() != Camera::Mode::ORBIT) {
    return;
  }

  if (App::view.getKey(GLFW_KEY_W) == GLFW_PRESS) {
    moved = true;
    App::camera.processKeyboard(Camera::Direction::FORWARD,
                                App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_S) == GLFW_PRESS) {
    moved = true;
    App::camera.processKeyboard(Camera::Direction::BACKWARD,
                                App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_A) == GLFW_PRESS) {
    moved = true;
    App::camera.processKeyboard(Camera::Direction::LEFT,
                                App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_D) == GLFW_PRESS) {
    moved = true;
    App::camera.processKeyboard(Camera::Direction::RIGHT,
                                App::view.getDeltaTime());
  }

  if (!moved) {
    App::camera.processKeyboard(Camera::Direction::NONE,
                                App::view.getDeltaTime());
  }
}
