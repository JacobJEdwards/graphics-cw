//
// Created by Jacob Edwards on 02/03/2024.
//
#include "App.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void setupGLFW();

View App::view;
bool App::paused = false;

CameraHolder App::cameras;

bool App::init() {
  setupGLFW();
  if (!view.isSetup() && !view.init("App", 800, 600)) {
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
  cameras.setAspect(static_cast<float>(width) / static_cast<float>(height));
  return true;
}

void App::loop() {
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
