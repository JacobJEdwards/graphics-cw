//
// Created by Jacob Edwards on 02/03/2024.
//
#include "App.h"

void setupGLFW();

View App::view;
Camera App::camera;
glm::mat4 App::projection = glm::mat4(1.0F);

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

template<typename F, typename... Args>
void App::loop(F&& func, Args&&... args) {
    while (!view.shouldClose()) {
        view.pollEvents();
        func(std::forward<Args>(args)...);
        view.render();
        view.swapBuffers();
    }
}

template<>
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


void App::calculateProjection() {
     projection =  glm::perspective(glm::radians(camera.getZoom()), static_cast<float>(view.getWidth()) / static_cast<float>(view.getHeight()), 0.1F, 100.0F);
}

void setupGLFW() {
    if (glfwInit() == 0) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
}