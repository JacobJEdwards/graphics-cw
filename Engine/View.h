//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_VIEW_H
#define CW_VIEW_H

#include <functional>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <memory>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"

#include "graphics/Color.h"

#include "graphics/PostProcessor.h"

class View {
    using Handle = std::function<void()>;
    using ErrorHandle = std::function<void(int, const char *)>;

public:
    View() = default;

    auto init(const std::string &title, int width, int height) -> bool;

    void quit() const;

    void render();

    void setMenu(Handle handle);

    void setInterface(Handle handle);

    void setKey(Handle handle);

    void setPipeline(Handle handle);

    void setMouse(Handle handle);

    void setScroll(Handle handle);

    void setResize(Handle handle);

    void setError(ErrorHandle handle);

    static void clearTarget(const glm::vec3 &color = Color::BLACK,
                            bool clearc = true, bool cleard = true);

    [[nodiscard]] auto shouldClose() const -> bool;

    void swapBuffers() const;

    static void pollEvents();

    [[nodiscard]] auto getWidth() const -> unsigned int { return WIDTH; }

    [[nodiscard]] auto getHeight() const -> unsigned int { return HEIGHT; }

    [[nodiscard]] auto getMouseX() const -> float { return lastX; }

    [[nodiscard]] auto getMouseY() const -> float { return lastY; }

    [[nodiscard]] auto getMouseOffsetX() const -> float { return xOffset; }

    [[nodiscard]] auto getMouseOffsetY() const -> float { return yOffset; }

    [[nodiscard]] auto getScrollX() const -> float { return scrollX; }

    [[nodiscard]] auto getScrollY() const -> float { return scrollY; }

    [[nodiscard]] auto shouldShowInterface() const -> bool {
        return showInterface;
    }

    void setShowInterface(const bool show) { showInterface = show; }

    void setShowMenu(const bool show) { showMenu = show; }

    [[nodiscard]] auto getDeltaTime() const -> float { return deltaTime; }

    [[nodiscard]] auto isSetup() const -> bool { return setup; }

    [[nodiscard]] auto getWindow() const -> GLFWwindow * { return window; }

    void setDimensions(const unsigned int width, const unsigned int height) {
        WIDTH = width;
        HEIGHT = height;
        (resize)();
    }

    [[nodiscard]] auto getPostProcessor() const -> PostProcess & { return *postProcessor; }

    void close() const;

    [[nodiscard]] auto getKey(int key) const -> int;

    void optionsInterface();

    void blurScreen() const {
        postProcessor->isBlurred();
    }

    [[nodiscard]] auto getTime() const -> float {
        return lastFrame;
    }

    bool highQuality = true;

private:
    GLFWwindow *window = nullptr;

    unsigned int WIDTH = 0U;
    unsigned int HEIGHT = 0U;
    std::string title;

    ImGuiIO io;

    std::unique_ptr<PostProcess> postProcessor{};
    bool postProcessorEnabled = true;

    bool showInterface = true;
    bool showMenu = true;

    void interfaceLoop() const;

    void menuLoop() const;

    void keyLoop() const;

    void setCallbacks() const;

    void scrollCallback(GLFWwindow *, double xOffset, double yOffset);

    void mouseCallback(GLFWwindow *, double x, double y);

    void errorCallback(int err, const char *description) const;

    void resizeCallback(GLFWwindow *, int width, int height);


    Handle interface = []() {
    };
    Handle menu = []() {
    };

    Handle key = []() {
    };
    Handle mouse = []() {
    };
    Handle pipeline = []() {
    };

    Handle scroll = []() {
    };

    Handle resize = [&]() {
    };

    ErrorHandle error = [](const int err, const char *description) {
        std::cerr << "Error: " << err << " - " << description << std::endl;
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

    bool wireframe = false;
};

#endif // CW_VIEW_H
