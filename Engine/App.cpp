//
// Created by Jacob Edwards on 02/03/2024.
//
#include "App.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "utils/PlayerHolder.h"

#include "Config.h"

void setupGLFW();

View App::view;
bool App::paused = false;
bool App::debug = false;

PlayerHolder App::players;

bool App::init() {
  setupGLFW();
  if (!view.isSetup() &&
      !view.init("App", Config::DEFAULT_WIDTH, Config::DEFAULT_HEIGHT)) {
    return false;
  }

  glViewport(0, 0, view.getWidth(), view.getHeight());
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_FRAMEBUFFER_SRGB);

  return true;
}

bool App::window(const std::string &title, unsigned int width,
                 unsigned int height) {
  if (!view.init(title, width, height)) {
    return false;
  }

  players.setAspect(static_cast<float>(width) / static_cast<float>(height));

  return true;
}

void App::finalise() {
  int width, height;

  glfwGetFramebufferSize(view.getWindow(), &width, &height);

  view.setDimensions(width, height);
}

void App::loop() {
  finalise();
  while (!view.shouldClose()) {
    view.pollEvents();
    view.render();
    view.swapBuffers();
  }
}

void App::quit() { view.quit(); }

void App::setPaused(const bool value) {
  paused = value;
  if (!paused) {
    glfwSetInputMode(view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else {
    glfwSetInputMode(App::view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

void setupGLFW() {
  if (glfwInit() == 0) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }
}
