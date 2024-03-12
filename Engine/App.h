//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_ENGINE_H
#define CW_ENGINE_H

#include <cstdlib>
#include <iostream>
#include <string>

#include "View.h"
#include "utils/Camera.h"

#include <GL/glew.h>
#include <glfw/glfw3.h>

namespace App {
    extern View view;
    extern Camera camera;
    extern glm::mat4 projection;

    extern bool paused;

    constexpr static unsigned int DEFAULT_WIDTH = 1200;
    constexpr static unsigned int DEFAULT_HEIGHT = 900;

    auto init() -> bool;

    auto window(const std::string &title, unsigned int width = DEFAULT_WIDTH,
                unsigned int height = DEFAULT_HEIGHT) -> bool;

    void quit();

    void calculateProjection();

    void setPaused(bool value);

    template<typename F, typename... Args>
    void loop(F &&func, Args &&... args) {
        while (!view.shouldClose()) {
            view.pollEvents();
            if (!paused) {
                func(std::forward<Args>(args)...);
            }
            view.render();
            view.swapBuffers();
        }
    }

    void loop();
}

#endif // CW_ENGINE_H
