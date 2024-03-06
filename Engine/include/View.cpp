//
// Created by Jacob Edwards on 02/03/2024.
//

#include "View.h"

/*
class View {
    using Handle = std::function<void()>;
    using KeyHandle = std::function<void(int, int, int, int)>;
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

    KeyHandle key = [](int key, int scancode, int action, int mods){};
    void keyLoop();

    Handle render = [](){};
    void renderLoop();
    void loop();
    void displayTarget(glm::vec3 color, bool clearc = true, bool cleard = true);

    void errorCallback(const int error, const char *description);

    bool shouldClose();
    void swapBuffers();
    void pollEvents();
};
*/

bool setupGLEW() {
        glewExperimental = GL_TRUE;
        if (GLEW_OK != glewInit()) {
            return false;
        }
        return true;
    }

GLFWwindow * createWindow(std::string title, int width, int height) {
    if (glfwInit() == 0) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    return window;
}



bool View::init(std::string title, int width, int height) {
    this->window = createWindow(title, width, height);
    this->WIDTH = width;
    this->HEIGHT = height;
    this->title = title;

    if (!setupGLEW()) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO();
    (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGui::StyleColorsDark();
    setup = true;

    return true;
}

void View::quit() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void View::interfaceLoop() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    interface();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void View::mouseLoop() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    mouse(x, y, 0);
}

void View::keyLoop() {
    key(window);
}

void View::displayTarget(glm::vec3 color, bool clearc, bool cleard) {
    if (clearc) {
        glClearColor(color.x, color.y, color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    if (cleard) {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
}

void View::render() {
    if (showInterface) { interfaceLoop(); }

    keyLoop();
    mouseLoop();
    pipeline();
}

bool shouldClose() {
    return glfwWindowShouldClose(window);
}

void swapBuffers() {
    glfwSwapBuffers(window);
}

void pollEvents() {
    glfwPollEvents();
}

