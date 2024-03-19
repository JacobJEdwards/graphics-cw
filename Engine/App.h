//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_ENGINE_H
#define CW_ENGINE_H

#include <string>

#include "Config.h"
#include "View.h"
#include "utils/CameraHolder.h"
#include "utils/PlayerHolder.h"

#include <GL/glew.h>
#include <glfw/glfw3.h>

namespace App {

extern View view;

extern PlayerHolder players;

extern bool paused;

extern bool debug;

extern bool wireframe;

// query retina display !!!!!!!
auto init() -> bool;

auto window(const std::string &title,
            unsigned int width = Config::DEFAULT_WIDTH,
            unsigned int height = Config::DEFAULT_HEIGHT) -> bool;

void quit();

void setPaused(bool value);

void finalise();

void debugInterface();

template <typename F, typename... Args> void loop(F &&func, Args &&...args) {
  finalise();
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
