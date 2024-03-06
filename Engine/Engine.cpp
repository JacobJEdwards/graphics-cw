//
// Created by Jacob Edwards on 02/03/2024.
//

#include "Engine.h"

void setupGLFW();
void setupGLEW();


namespace Engine {
    bool init() {
        setupGLFW();
        if (!view.setup && !view.init("Engine", 800, 600)) {
            return false;
        }

        return true;
    }

    bool window(std::string title, int width, int height) {
        if (!view.init(title, width, height)) {
            return false;
        }
        return true;
    }

    void quit() {
        view.quit();
    }
}

void setupGLFW() {
        if (glfwInit() == 0) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            exit(EXIT_FAILURE);
        }
}

