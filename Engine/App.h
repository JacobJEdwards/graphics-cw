//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_ENGINE_H
#define CW_ENGINE_H

#include <string>

#include "Config.h"
#include "View.h"

namespace App {
    extern View view;

    extern bool paused;

    extern bool debug;

    // query retina display !!!!!!!
    auto init() -> bool;

    auto window(const std::string &title,
                int width = Config::DEFAULT_WIDTH,
                int height = Config::DEFAULT_HEIGHT) -> bool;

    void quit();

    void setPaused(bool value);

    void finalise();

    void debugInterface();

    template<typename F, typename... Args>
    void loop(F &&func, Args &&... args) {
        finalise();
        while (!view.shouldClose()) {
            View::pollEvents();
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
