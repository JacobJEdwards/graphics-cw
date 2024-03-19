//
// Created by Jacob Edwards on 02/03/2024.
//

#include "View.h"

#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include "graphics/PostProcessor.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

auto setupGLEW() -> bool {
  glewExperimental = GL_TRUE;
  return GLEW_OK == glewInit();
}

auto createWindow(const std::string &title, int width, int height)
    -> GLFWwindow * {
  if (glfwInit() == 0) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
  glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);

  GLFWwindow *window =
      glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
  if (window == nullptr) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  return window;
}

void View::setCallbacks() {
  glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y) {
    static_cast<View *>(glfwGetWindowUserPointer(window))
        ->mouseCallback(window, x, y);
  });

  glfwSetScrollCallback(window, [](GLFWwindow *window, double x, double y) {
    static_cast<View *>(glfwGetWindowUserPointer(window))
        ->scrollCallback(window, x, y);
  });

  glfwSetErrorCallback([](int error, const char *description) {
    static_cast<View *>(glfwGetWindowUserPointer(glfwGetCurrentContext()))
        ->errorCallback(error, description);
  });

  glfwSetFramebufferSizeCallback(
      window, [](GLFWwindow *window, int width, int height) {
        static_cast<View *>(glfwGetWindowUserPointer(window))
            ->resizeCallback(window, width, height);
      });
}

auto View::init(const std::string &title, int width, int height) -> bool {
  this->window = createWindow(title, width, height);
  this->WIDTH = width;
  this->HEIGHT = height;
  this->title = title;

  if (!setupGLEW()) {
    std::cerr << "Failed to initialize GLEW" << std::endl;
    return false;
  }

  glfwSetWindowUserPointer(window, this);

  setCallbacks();

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = ImGui::GetIO();

  (void)io;

  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 410");
  ImGui::StyleColorsDark();
  setup = true;

  auto postProcessingShader =
      std::make_shared<Shader>("../assets/shaders/postProcessing.vert",
                               "../assets/shaders/postProcessing.frag");
  postProcessor = std::make_unique<PostProcess>(
      WIDTH, HEIGHT, postProcessingShader, multiSample);

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
  (interface)();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void View::menuLoop() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  (menu)();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void View::mouseCallback(GLFWwindow * /*window*/, double x, double y) {
  io.MousePos.x = static_cast<float>(x);
  io.MousePos.y = static_cast<float>(y);

  if (firstMouse) {
    lastX = static_cast<float>(x);
    lastY = static_cast<float>(y);
    firstMouse = false;
  }

  xOffset = static_cast<float>(x) - lastX;
  yOffset = lastY - static_cast<float>(y);

  lastX = static_cast<float>(x);
  lastY = static_cast<float>(y);

  (mouse)();
}

void View::keyLoop() { (key)(); }

void View::scrollCallback(GLFWwindow * /*window*/, double xoffset,
                          double yoffset) {
  scrollX = static_cast<float>(xoffset);
  scrollY = static_cast<float>(yoffset);

  (scroll)();
}

void View::errorCallback(const int err, const char *description) {
  (error)(err, description);
}

void View::resizeCallback(GLFWwindow * /*window*/, int width, int height) {
  WIDTH = width;
  HEIGHT = height;
  postProcessor->resize(width, height);
  glViewport(0, 0, width, height);

  (resize)();
}

void View::clearTarget(const glm::vec3 &color, const bool clearc,
                       const bool cleard) {
  glClearColor(color.x, color.y, color.z, 1.0F);

  if (clearc) {
    glClear(GL_COLOR_BUFFER_BIT);
  }
  if (cleard) {
    glClear(GL_DEPTH_BUFFER_BIT);
  }
}

void View::render() {
  const auto currentFrame = static_cast<float>(glfwGetTime());
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  keyLoop();

  if (postProcessorEnabled) {
    postProcessor->begin();
    (pipeline)();
    postProcessor->end();

    postProcessor->render();
  } else {
    (pipeline)();
  }

  if (showInterface) {
    interfaceLoop();
  }
}

auto View::shouldClose() -> bool { return glfwWindowShouldClose(window) != 0; }

void View::swapBuffers() { glfwSwapBuffers(window); }

void View::pollEvents() { glfwPollEvents(); }

void View::setInterface(Handle handle) { interface = std::move(handle); }

void View::setMenu(Handle handle) { menu = std::move(handle); }

void View::setKey(Handle handle) { key = std::move(handle); }

void View::setPipeline(Handle handle) { pipeline = std::move(handle); }

void View::setMouse(Handle handle) { mouse = std::move(handle); }

void View::setScroll(Handle handle) { scroll = std::move(handle); }

void View::setResize(Handle handle) { resize = std::move(handle); }

void View::setError(ErrorHandle handle) { error = std::move(handle); }

void View::close() const { glfwSetWindowShouldClose(window, GLFW_TRUE); }

auto View::getKey(int key) const -> int { return glfwGetKey(window, key); }

void View::optionsInterface() {
  ImGui::Begin("View Options");
  ImGui::Checkbox("Post Processing", &postProcessorEnabled);

  if (postProcessorEnabled) {
    bool sample = multiSample;
    ImGui::Checkbox("Multi Sample", &sample);
    if (sample != multiSample) {
      multiSample = sample;
      postProcessor = std::make_unique<PostProcess>(
          WIDTH, HEIGHT, postProcessor->getShader(), multiSample);
    }
  }

  if (ImGui::Button("Close")) {
    close();
  }

  ImGui::End();
}
