//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_ENGINE_H
#define CW_ENGINE_H

#include <iostream>
#include <functional>
#include <initializer_list>
#include <string>
#include <filesystem>
#include <sstream>

#include "./include/View.h"

#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using Handle = std::function<void()>;

namespace Engine {
    static View view;

    bool init();

    bool window(std::string title, int width, int height);

    void quit();

    template <typename F, typename... Args>
    void loop(F&& f, Args&&... args) {
        while (!view.shouldClose() {
            view.clearTarget(glm::vec3(0.0f, 0.0f, 0.0f));
            view.render();

            f(std::forward<Args>(args)...);
            view.swapBuffers();
            view.pollEvents();
        }
    }
}

#endif //CW_ENGINE_H
