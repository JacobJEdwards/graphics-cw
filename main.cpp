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

#include "Config.h"

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

void processInput();

auto main() -> int {
  App::window("Coursework", Config::DEFAULT_WIDTH, Config::DEFAULT_HEIGHT);
  App::init();

  App::view.setKey(processInput);
  App::view.setMouse([&] {
    if (!App::paused) {
      App::cameras.getActiveCamera()->processMouseMovement(
          App::view.getMouseOffsetX(), App::view.getMouseOffsetY());
    }
  });

  App::view.setScroll([&] {
    App::cameras.getActiveCamera()->processMouseScroll(App::view.getScrollY());
  });

  App::view.setResize([&] {
    App::cameras.getActiveCamera()->setAspect(
        static_cast<float>(App::view.getWidth()) / App::view.getHeight());
    glViewport(0, 0, static_cast<GLsizei>(App::view.getWidth()),
               static_cast<GLsizei>(App::view.getHeight()));
  });

  App::view.setMenu([&] {
    glfwSetInputMode(App::view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    View::clearTarget(Color::BLACK);
  });

  App::view.setShowMenu(false);

  const std::array<std::string, 6> skyboxFaces{
      "../Assets/textures/skybox/right.jpg",
      "../Assets/textures/skybox/left.jpg",
      "../Assets/textures/skybox/top.jpg",
      "../Assets/textures/skybox/bottom.jpg",
      "../Assets/textures/skybox/front.jpg",
      "../Assets/textures/skybox/back.jpg",
  };

  const Skybox skybox(skyboxFaces);
  Sun sun;

  Texture::Loader::setFlip(false);
  Model newModel("../Assets/objects/helecopter/chopper.obj");
  Texture::Loader::setFlip(true);
  Model model2("../Assets/objects/backpack/backpack.obj");
  Texture::Loader::setFlip(false);

  Player person;

  auto orbitCamera = std::make_shared<Camera>(glm::vec3(0.0F, 0.0F, 0.0F));
  orbitCamera->setOrbit(glm::vec3(0.0F, 0.0F, 0.0F), 10.0F, 5.0F, 5.0F);
  orbitCamera->setMode(Camera::Mode::ORBIT);
  App::cameras.addCamera("Orbit", orbitCamera);

  auto freeCamera = std::make_shared<Camera>(glm::vec3(0.0F, 0.0F, 0.0F));
  freeCamera->setMode(Camera::Mode::FREE);
  App::cameras.addCamera("Free", freeCamera);

  auto fpsCamera = std::make_shared<Camera>(glm::vec3(0.0F, 0.0F, 0.0F));
  fpsCamera->setMode(Camera::Mode::FPS);
  App::cameras.addCamera("FPS", fpsCamera);

  auto fixedCamera = std::make_shared<Camera>(glm::vec3(0.0F, 0.0F, 0.0F));
  fixedCamera->setFixed(glm::vec3(0.0F, 0.0F, 0.0F),
                        glm::vec3(0.0F, 1.0F, 0.0F));
  fixedCamera->setMode(Camera::Mode::FIXED);
  App::cameras.addCamera("Fixed", fixedCamera);

  App::cameras.setActiveCamera("Orbit");

  InfinitePlane terrain;

  auto ourShader = std::make_shared<Shader>("../assets/shaders/backpack.vert",
                                            "../assets/shaders/backpack.frag");

  newModel.setShader(ourShader);
  model2.setShader(ourShader);

  ourShader->use();
  ourShader->setUniform("projection",
                        App::cameras.getActiveCamera()->getProjectionMatrix());
  ourShader->setUniform("light.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
  ourShader->setUniform("light.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
  ourShader->setUniform("light.specular", glm::vec3(1.0F, 1.0F, 1.0F));
  // material properties
  ourShader->setUniform("material.specular", glm::vec3(0.5F, 0.5F, 0.5F));
  ourShader->setUniform("material.shininess", 64.0F);

  auto model = glm::mat4(1.0F);
  glm::vec3 newPosition = App::cameras.getActiveCamera()->getPosition() +
                          glm::vec3(0.0F, 8.0F, 5.0F);
  const glm::mat4 helicopterModel =
      translate(model, newPosition) *
      rotate(model, glm::radians(90.0F), glm::vec3(1.0F, 0.0F, 0.0F));

  // newModel.scale(glm::vec3(0.1F, 0.1F, 0.1F));
  // newModel.translate(newPosition);
  newModel.transform(helicopterModel);

  model = Config::IDENTITY_MATRIX;
  newPosition = App::cameras.getActiveCamera()->getPosition() +
                glm::vec3(5.0F, 3.0F, 0.45F);
  const glm::mat4 backpackModel = translate(model, newPosition);

  model2.transform(backpackModel);
  // scale(glm::mat4(1.0F), glm::vec3(0.1F, 0.1F, 0.1F));

  sun.setPosition(App::cameras.getActiveCamera()->getPosition());

  App::view.setPipeline([&]() {
    const auto projectionMatrix =
        App::cameras.getActiveCamera()->getProjectionMatrix();

    View::clearTarget(Color::BLACK);

    ourShader->use();

    ourShader->setUniform("projection", projectionMatrix);

    ourShader->setUniform("light.position", glm::vec3(sun.getPosition(), 1.0F));
    // view/projection transformations
    glm::mat4 view = App::cameras.getActiveCamera()->getViewMatrix();
    ourShader->setUniform("view", view);
    ourShader->setUniform("viewPos",
                          App::cameras.getActiveCamera()->getPosition());

    newModel.draw();

    // go upwards
    // newModel.attributes.applyForce(glm::vec3(0.0F, 1.0F, 0.0F));

    if (newModel.detectCollisions(
            App::cameras.getActiveCamera()->getPosition())) {
      Physics::calculateCollisionResponse(
          App::cameras.getActiveCamera()->attributes, newModel.attributes);
    }

    model2.draw();

    if (model2.detectCollisions(
            App::cameras.getActiveCamera()->getPosition())) {
      Physics::calculateCollisionResponse(
          App::cameras.getActiveCamera()->attributes, model2.attributes);
    }

    terrain.draw(view, projectionMatrix, glm::vec3(sun.getPosition(), 1.0F),
                 App::cameras.getActiveCamera()->getPosition());

    if (terrain.detectCollisions(
            App::cameras.getActiveCamera()->getPosition())) {
      Physics::calculateCollisionResponseFloor(
          App::cameras.getActiveCamera()->attributes, terrain.attributes);
      App::cameras.getActiveCamera()->attributes.isGrounded = true;
    } else {
      App::cameras.getActiveCamera()->attributes.isGrounded = false;
    }

    view =
        glm::mat4(glm::mat3(App::cameras.getActiveCamera()->getViewMatrix()));
    skybox.draw(projectionMatrix, view, sun.getPosition().y);
    sun.draw(view, projectionMatrix);

    person.draw(App::cameras.getActiveCamera()->getViewMatrix(),
                projectionMatrix);
  });

  App::view.setInterface([&]() {
    if (App::paused) {
      App::cameras.getActiveCamera()->controlInterface();
      App::cameras.interface();
    }
  });

  try {
    App::loop([&] {
      sun.update(App::view.getDeltaTime());
      App::cameras.getActiveCamera()->circleOrbit(App::view.getDeltaTime());
      App::cameras.getActiveCamera()->update(App::view.getDeltaTime());
      person.update();
      newModel.update(App::view.getDeltaTime());
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

  if (App::paused &&
      App::cameras.getActiveCamera()->getMode() != Camera::Mode::ORBIT) {
    return;
  }

  if (App::view.getKey(GLFW_KEY_W) == GLFW_PRESS) {
    moved = true;
    App::cameras.getActiveCamera()->processKeyboard(Camera::Direction::FORWARD,
                                                    App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_S) == GLFW_PRESS) {
    moved = true;
    App::cameras.getActiveCamera()->processKeyboard(Camera::Direction::BACKWARD,
                                                    App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_A) == GLFW_PRESS) {
    moved = true;
    App::cameras.getActiveCamera()->processKeyboard(Camera::Direction::LEFT,
                                                    App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_D) == GLFW_PRESS) {
    moved = true;
    App::cameras.getActiveCamera()->processKeyboard(Camera::Direction::RIGHT,
                                                    App::view.getDeltaTime());
  }

  if (!moved) {
    App::cameras.getActiveCamera()->processKeyboard(Camera::Direction::NONE,
                                                    App::view.getDeltaTime());
  }
}
