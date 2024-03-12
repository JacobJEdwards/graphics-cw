//
// Created by Jacob Edwards on 02/03/2024.
//
#include "App.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Config.h"

void setupGLFW();

View App::view;
Camera App::camera;
glm::mat4 App::projection = Config::IDENTITY_MATRIX;
bool App::paused = false;

bool App::init() {
    setupGLFW();
    if (!view.isSetup() && !view.init("App", 800, 600)) {
        return false;
    }

    calculateProjection();

    glViewport(0, 0, view.getWidth(), view.getHeight());
    glEnable(GL_DEPTH_TEST);

    return true;
}

bool App::window(const std::string& title, unsigned int width, unsigned int height) {
    if (!view.init(title, width, height)) {
        return false;
    }
    return true;
}

void App::loop() {
    while (!view.shouldClose()) {
        view.pollEvents();
        view.render();
        view.swapBuffers();
    }
}

void App::quit() {
    view.quit();
}

void App::setPaused(const bool value) {
    paused = value;
    if (!paused) {
        glfwSetInputMode(view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(App::view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void App::calculateProjection() {
     projection = glm::perspective(glm::radians(camera.getZoom()), static_cast<float>(view.getWidth()) / static_cast<float>(view.getHeight()), 0.1F, 100.0F);
}

void setupGLFW() {
    if (glfwInit() == 0) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
}