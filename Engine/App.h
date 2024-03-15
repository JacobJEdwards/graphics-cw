//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_ENGINE_H
#define CW_ENGINE_H

#include <string>

#include "Config.h"
#include "View.h"
#include "utils/CameraHolder.h"

#include <GL/glew.h>
#include <glfw/glfw3.h>

namespace App {

extern View view;

extern CameraHolder cameras;

extern bool paused;

auto init() -> bool;

auto window(const std::string &title,
            unsigned int width = Config::DEFAULT_WIDTH,
            unsigned int height = Config::DEFAULT_HEIGHT) -> bool;

void quit();

void setPaused(bool value);

template <typename F, typename... Args> void loop(F &&func, Args &&...args) {
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
} // namespace App

#endif // CW_ENGINE_H
