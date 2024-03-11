//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_VIEW_H
#define CW_VIEW_H

#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <string>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"

#include "graphics/Color.h"

class View {
    using Handle = std::function<void()>;
    using ErrorHandle = std::function<void(int, const char *)>;
public:
    View() = default;

    auto init(const std::string& title, int width, int height) -> bool;
    void quit();
    void render();

    void setMenu(Handle handle);
    void setInterface(Handle handle);
    void setKey(Handle handle);
    void setPipeline(Handle handle);
    void setMouse(Handle handle);
    void setScroll(Handle handle);
    void setResize(Handle handle);
    void setError(ErrorHandle handle);

    static void clearTarget(const glm::vec3 &color = Color::BLACK, bool clearc = true, bool cleard = true);

    auto shouldClose() -> bool;
    void swapBuffers();
    static void pollEvents();

    [[nodiscard]] auto getWidth() const -> unsigned int { return WIDTH; }
    [[nodiscard]] auto getHeight() const -> unsigned int { return HEIGHT; }

    [[nodiscard]] auto getMouseX() const -> float { return lastX; }
    [[nodiscard]] auto getMouseY() const -> float { return lastY; }

    [[nodiscard]] auto getMouseOffsetX() const -> float { return xOffset; }
    [[nodiscard]] auto getMouseOffsetY() const -> float { return yOffset; }

    [[nodiscard]] auto getScrollX() const -> float { return scrollX; }
    [[nodiscard]] auto getScrollY() const -> float { return scrollY; }

    [[nodiscard]] auto shouldShowInterface() const -> bool { return showInterface; }

    void setShowInterface(bool show) { showInterface = show; }
    void setShowMenu(bool show) { showMenu = show; }

    [[nodiscard]] auto getDeltaTime() const -> float { return deltaTime; }

    [[nodiscard]] auto isSetup() const -> bool {
        return setup;
    }

    [[nodiscard]] auto getWindow() const -> GLFWwindow* {
        return window;
    }

private:
    GLFWwindow *window = nullptr;

    unsigned int WIDTH = 0;
    unsigned int HEIGHT = 0;
    std::string title;

    ImGuiIO io;

    bool showInterface = true;
    bool showMenu = true;
    void interfaceLoop();
    void menuLoop();
    void keyLoop();

    void setCallbacks();

    void scrollCallback(GLFWwindow*, double x, double y);
    void mouseCallback(GLFWwindow*, double x, double y);
    void errorCallback(int error, const char* description);
    void resizeCallback(GLFWwindow*, int width, int height);

    Handle interface = []() {};
    Handle menu = []() {};

    Handle key = []() {};
    Handle mouse = []() {};
    Handle pipeline = []() {};

    Handle scroll = []() {};

    ErrorHandle error = [](int err, const char* description) {
        std::cerr << "Error: " << err << " - " << description << std::endl;
    };

    Handle resize = [&]() {
        glViewport(0, 0, static_cast<GLsizei>(WIDTH), static_cast<GLsizei>(HEIGHT));
    };

    bool setup = false;

    // mouse
    bool firstMouse = true;

    float lastX = 0.0F;
    float lastY = 0.0F;

    float xOffset = 0.0F;
    float yOffset = 0.0F;

    // scroll
    float scrollX = 0.0F;
    float scrollY = 0.0F;

    // time
    float deltaTime = 0.0F;
    float lastFrame = 0.0F;
};


#endif //CW_VIEW_H
