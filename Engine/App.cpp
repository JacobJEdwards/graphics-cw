//
// Created by Jacob Edwards on 02/03/2024.
//
#include "App.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <print>

#include "imgui/imgui.h"
#include "utils/PlayerManager.h"
#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include "View.h"
#include "Config.h"

void setupGLFW();

View App::view;
bool App::paused = false;
bool App::debug = false;

auto App::init() -> bool {
    setupGLFW();
    if (!view.isSetup() &&
        !view.init("App", Config::DEFAULT_WIDTH, Config::DEFAULT_HEIGHT)) {
        return false;
    }

    glViewport(0, 0, static_cast<GLsizei>(view.getWidth()), static_cast<GLsizei>(view.getHeight()));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    // blend func options
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    // glBlendFunc(GL_ONE, GL_ONE);
    // glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


    glEnable(GL_FRAMEBUFFER_SRGB);
    // glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    return true;
}

auto App::window(const std::string &title, const int width,
                 const int height) -> bool {
    if (!view.init(title, width, height)) {
        return false;
    }

    PlayerManager::GetInstance().setAspect(static_cast<float>(width) / static_cast<float>(height));

    return true;
}

void App::finalise() {
    int width;
    int height;

    glfwGetFramebufferSize(view.getWindow(), &width, &height);

    view.setDimensions(view.getWidth(), view.getHeight());
}


void App::loop() {
    finalise();
    while (!view.shouldClose()) {
        View::pollEvents();
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
        glfwSetInputMode(view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void setupGLFW() {
    if (glfwInit() == 0) {
        std::println(stderr, "Failed to initialize GLFW");
        exit(EXIT_FAILURE);
    }
}

void App::debugInterface() {
    ImGui::Begin("Debug");
    ImGui::Checkbox("Debug Mode", &debug);

    if (debug) {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    }

    ImGui::End();
}
