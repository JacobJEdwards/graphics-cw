//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_VIEW_H
#define CW_VIEW_H

#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_glfw.h"
#include "external/imgui/imgui_impl_opengl3.h"
#include <functional>

class View {
    using Handle = std::function<void()>;
    using KeyHandle = std::function<void(glfwWindow*)>;
    using MouseHandle = std::function<void(int, int)>;
public:
    bool init(std::string title, int width, int height);
    void quit();

    GLFWwindow *window;
    unsigned int WIDTH;
    unsigned int HEIGHT;
    std::string title;

    ImGuiIO io;
    Handle interface = [](){};
    bool showInterface = false;
    void interfaceLoop();

    KeyHandle key = [](glfwWindow*){};
    void keyLoop();

    Handle pipeline = [](){};
    void render();
    void displayTarget(glm::vec3 color, bool clearc = true, bool cleard = true);

    MouseHandle mouse = [](unsigned int x, unsigned int y){};
    void mouseLoop();

    void errorCallback(const int error, const char *description) const {
        std::cerr << "Error: " << error << " - " << description << std::endl;
    }

    bool shouldClose();
    void swapBuffers();
    void pollEvents();

    bool setup = false;
};


#endif //CW_VIEW_H
