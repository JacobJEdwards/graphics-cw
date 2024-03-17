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
#include "graphics/FrameBuffer.h"
#include "physics/Constants.h"
#include "utils/Camera.h"

#include "graphics/Color.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"
#include "physics/Collisions.h"
#include "shapes/Plane.h"
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
      App::players.getCurrent()->getCamera().processMouseMovement(
          App::view.getMouseOffsetX(), App::view.getMouseOffsetY());
    }
  });

  App::view.setScroll([&] {
    App::players.getCurrent()->getCamera().processMouseScroll(
        App::view.getScrollY());
  });

  App::view.setResize([&] {
    App::players.setAspect(static_cast<float>(App::view.getWidth()) /
                           App::view.getHeight());
    glViewport(0, 0, static_cast<GLsizei>(App::view.getWidth()),
               static_cast<GLsizei>(App::view.getHeight()));
    App::view.getFrameBuffer().resize(App::view.getWidth(),
                                      App::view.getHeight());
  });

  App::view.setMenu([&] {
    glfwSetInputMode(App::view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    View::clearTarget(Color::BLACK);
  });

  App::view.setShowMenu(false);

  App::view.resize();

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
  newModel.attributes.mass = 50.0F;
  Texture::Loader::setFlip(true);
  Model model2("../Assets/objects/backpack/backpack.obj");
  model2.attributes.mass = 0.1F;
  Texture::Loader::setFlip(false);

  auto orbit = std::make_shared<Player>();
  orbit->setDrawModel(false);
  orbit->getCamera().setMode(Camera::Mode::ORBIT);
  orbit->getCamera().setOrbit(glm::vec3(0.0F, 0.0F, 0.0F), 50.0F, 0.0F, 3.0F);
  App::players.add("Orbit", orbit);

  auto free = std::make_shared<Player>();
  free->setDrawModel(true);
  free->setMode(Player::Mode::FREE);
  App::players.add("Free", free);

  auto fps = std::make_shared<Player>();
  fps->setDrawModel(true);
  fps->setMode(Player::Mode::FPS);
  App::players.add("FPS", fps);

  auto fixed = std::make_shared<Player>();
  fixed->setDrawModel(true);
  fixed->getCamera().setFixed(glm::vec3(0.0F, 0.0F, 0.0F),
                              glm::vec3(4.0F, 3.0F, 6.0F));
  fixed->setMode(Player::Mode::FIXED);
  App::players.add("Fixed", fixed);

  App::players.setCurrent("Orbit");

  InfinitePlane terrain;

  Plane postProcessingPlane;

  auto ourShader = std::make_shared<Shader>("../assets/shaders/backpack.vert",
                                            "../assets/shaders/backpack.frag");

  auto postProcessingShader =
      std::make_shared<Shader>("../assets/shaders/postProcessing.vert",
                               "../assets/shaders/postProcessing.frag");

  postProcessingShader->use();
  postProcessingShader->setUniform("screenTexture", 0);

  newModel.setShader(ourShader);
  model2.setShader(ourShader);

  ourShader->use();

  ourShader->setUniform("light.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
  ourShader->setUniform("light.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
  ourShader->setUniform("light.specular", glm::vec3(1.0F, 1.0F, 1.0F));
  // material properties
  ourShader->setUniform("material.specular", glm::vec3(0.5F, 0.5F, 0.5F));
  ourShader->setUniform("material.shininess", 64.0F);

  auto model = glm::mat4(1.0F);

  glm::vec3 newPosition = App::players.getCurrent()->getCamera().getPosition() +
                          glm::vec3(0.0F, 15.0F, 10.0F);

  const glm::mat4 helicopterModel = translate(model, newPosition);

  // newModel.scale(glm::vec3(0.1F, 0.1F, 0.1F));
  // newModel.translate(newPosition);
  newModel.transform(helicopterModel);

  model = Config::IDENTITY_MATRIX;
  newPosition = App::players.getCurrent()->getCamera().getPosition() +
                glm::vec3(5.0F, 2.0F, 0.45F);

  const glm::mat4 backpackModel = translate(model, newPosition);

  model2.transform(backpackModel);
  // scale(glm::mat4(1.0F), glm::vec3(0.1F, 0.1F, 0.1F));

  sun.setPosition(App::players.getCurrent()->getCamera().getPosition());

  // obviously clean this up

  float quadVertices[] = {// vertex attributes for a quad that fills the entire
                          // screen in Normalized Device Coordinates.
                          // positions   // texCoords
                          -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                          0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

                          -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                          1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};
  // screen quad VAO
  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));

  App::view.setPipeline([&]() {
    App::view.getFrameBuffer().bind();
    glEnable(GL_DEPTH_TEST);
    View::clearTarget(Color::BLACK);
    auto player = App::players.getCurrent();

    const auto projectionMatrix = player->getCamera().getProjectionMatrix();

    const auto viewMatrix = player->getCamera().getViewMatrix();

    ourShader->use();

    ourShader->setUniform("projection", projectionMatrix);

    ourShader->setUniform("light.position", glm::vec3(sun.getPosition(), 1.0F));
    ourShader->setUniform("view", viewMatrix);

    ourShader->setUniform("viewPos", player->getCamera().getPosition());

    newModel.draw();
    model2.draw();
    terrain.draw(viewMatrix, projectionMatrix,
                 glm::vec3(sun.getPosition(), 1.0F),
                 player->getCamera().getPosition());

    skybox.draw(projectionMatrix, viewMatrix, sun.getPosition().y);
    sun.draw(viewMatrix, projectionMatrix);
    App::players.draw(viewMatrix, projectionMatrix);

    App::view.getFrameBuffer().unbind();

    glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't
                              // discarded due to depth test.
    // clear all relevant buffers
    glClearColor(
        1.0f, 1.0f, 1.0f,
        1.0f); // set clear color to white (not really necessary actually, since
               // we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);

    postProcessingShader->use();
    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D,
                  App::view.getFrameBuffer()
                      .getTexture()); // use the color attachment texture as the
                                      // texture of the quad plane
    glDrawArrays(GL_TRIANGLES, 0, 6);
  });

  App::view.setInterface([&]() {
    if (App::paused) {
      App::players.getCurrent()->getCamera().interface();
      App::players.interface();
      App::players.getCurrent()->interface();

      ImGui::Begin("Debug");
      ImGui::Checkbox("Debug Mode", &App::debug);
      ImGui::End();
    }
  });

  try {
    App::loop([&] {
      auto player = App::players.getCurrent();
      if (Physics::Collisions::check(player->getBoundingBox(),
                                     terrain.getBoundingBox())) {
        Physics::Collisions::resolve(player->getAttributes(),
                                     Physics::FLOOR_NORMAL);
        player->getAttributes().isGrounded = true;

      } else {
        player->getAttributes().isGrounded = false;
      }

      if (Physics::Collisions::check(newModel.getBoundingBox(),
                                     terrain.getBoundingBox())) {
        Physics::Collisions::resolve(newModel.attributes,
                                     Physics::FLOOR_NORMAL);
        newModel.attributes.isGrounded = true;
      } else {
        newModel.attributes.isGrounded = false;
      }

      if (Physics::Collisions::check(model2.getBoundingBox(),
                                     terrain.getBoundingBox())) {
        Physics::Collisions::resolve(model2.attributes, Physics::FLOOR_NORMAL);
        model2.attributes.isGrounded = true;
      } else {
        model2.attributes.isGrounded = false;
      }

      if (Physics::Collisions::check(newModel.getBoundingBox(),
                                     model2.getBoundingBox())) {
        Physics::Collisions::resolve(newModel.attributes, model2.attributes);
      }

      if (Physics::Collisions::check(player->getBoundingBox(),
                                     newModel.getBoundingBox())) {
        Physics::Collisions::resolve(player->getAttributes(),
                                     newModel.attributes);
      }

      if (Physics::Collisions::check(player->getBoundingBox(),
                                     model2.getBoundingBox())) {
        Physics::Collisions::resolve(player->getAttributes(),
                                     model2.attributes);
      }

      sun.update(App::view.getDeltaTime());
      App::players.getCurrent()->update(App::view.getDeltaTime());
      newModel.update(App::view.getDeltaTime());
      model2.update(App::view.getDeltaTime());
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
      App::players.getCurrent()->getCamera().getMode() != Camera::Mode::ORBIT) {
    return;
  }

  if (App::view.getKey(GLFW_KEY_W) == GLFW_PRESS) {
    moved = true;
    App::players.getCurrent()->processKeyboard(Player::Direction::FORWARD,
                                               App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_S) == GLFW_PRESS) {
    moved = true;
    App::players.getCurrent()->processKeyboard(Player::Direction::BACKWARD,
                                               App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_A) == GLFW_PRESS) {
    moved = true;
    App::players.getCurrent()->processKeyboard(Player::Direction::LEFT,
                                               App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_D) == GLFW_PRESS) {
    moved = true;
    App::players.getCurrent()->processKeyboard(Player::Direction::RIGHT,
                                               App::view.getDeltaTime());
  }

  if (!moved) {
    App::players.getCurrent()->processKeyboard(Player::Direction::NONE,
                                               App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    App::players.getCurrent()->processKeyboard(Player::Direction::UP,
                                               App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
    App::players.getCurrent()->processKeyboard(Player::Direction::DOWN,
                                               App::view.getDeltaTime());
  }

  if (App::view.getKey(GLFW_KEY_N) == GLFW_PRESS) {
    App::players.getCurrent()->nitro();
  }

  if (App::view.getKey(GLFW_KEY_J) == GLFW_PRESS) {
    if (App::players.getCurrent()->getAttributes().isGrounded) {
      App::players.getCurrent()->jump();
    }
  }
}
