#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_glfw.h"
#include "external/imgui/imgui_impl_opengl3.h"

#include "Graphics/Shader.h"
#include "Game/Camera.h"
#include "Graphics/Texture.h"
#include "Graphics/Model.h"
#include "Graphics/Skybox.h"
#include "Graphics/ProceduralTerrain.h"
#include "Graphics/InfinitePlane.h"
#include "Graphics/Sun.h"


Shader *ourShader;

constexpr unsigned int SCR_WIDTH = 1200;
constexpr unsigned int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0F, 0.0F, 3.0F));
bool useMouse = false;
bool updateSun = true;

bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0F;
float lastY = SCR_HEIGHT / 2.0F;

float deltaTime = 0.0F;
float lastFrame = 0.0F;

glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()),static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1F, 100.0F);

// function prototypes
void errorCallback(int error, const char *description);

void setupGLFW();

void setupGLEW();

void reshape(GLFWwindow *window, int width, int height);

auto createWindow(int width, int height, const char *title) -> GLFWwindow *;

void keyCallback(GLFWwindow *window, int key, int code, int action, int mod);

void cleanup(GLFWwindow *window);

void processInput(GLFWwindow *window);

void mouseCallback(GLFWwindow *window, double xpos, double ypos);

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

auto main() -> int {
    setupGLFW();
    GLFWwindow *window = createWindow(SCR_WIDTH, SCR_HEIGHT, "Coursework");
    setupGLEW();

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);


    const std::vector<std::string> skyboxFaces {
        "../Assets/textures/skybox/right.jpg",
        "../Assets/textures/skybox/left.jpg",
        "../Assets/textures/skybox/top.jpg",
        "../Assets/textures/skybox/bottom.jpg",
        "../Assets/textures/skybox/front.jpg",
        "../Assets/textures/skybox/back.jpg",
    };

    Skybox skybox(skyboxFaces);
    Sun sun;


    Texture::Loader::setFlip(false);
    const Model newModel("../Assets/objects/helecopter/chopper.obj");
    Texture::Loader::setFlip(true);
    const Model model2("../Assets/objects/backpack/backpack.obj");
    Texture::Loader::setFlip(false);

    const InfinitePlane terrain;
    ourShader = new Shader("../Assets/shaders/backpack.vert", "../Assets/shaders/backpack.frag");

    //camera.setPosition(glm::vec3(terrain.getOriginX(), 20.0F, terrain.getOriginY()));

    projection = glm::perspective(glm::radians(camera.getZoom()), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1F, 200.0F);
   // projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, (float)terrain.getChunkWidth() * (terrain.getRenderDistance() - 1.2F));


    ourShader->use();
    ourShader->setUniform("projection", projection);
    ourShader->setUniform("light.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
    ourShader->setUniform("light.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
    ourShader->setUniform("light.specular", glm::vec3(1.0F, 1.0F, 1.0F));
    // material properties
    ourShader->setUniform("material.specular", glm::vec3(0.5F, 0.5F, 0.5F));
    ourShader->setUniform("material.shininess", 64.0F);


    auto model = glm::mat4(1.0F);
    glm::vec3 newPosition = camera.getPosition() + glm::vec3(0.0F, -0.15F, 0.45F);
    glm::mat4 helicopterModel = translate(model, newPosition) * scale(glm::mat4(1.0F), glm::vec3(0.3F, 0.3F, 0.3F));

    ourShader->setUniform("model", helicopterModel);

    model = glm::mat4(1.0F);
    newPosition = camera.getPosition() + glm::vec3(5.0F, -0.15F, 0.45F);
    glm::mat4 backpackModel = translate(model, newPosition) * scale(glm::mat4(1.0F), glm::vec3(0.1F, 0.1F, 0.1F));


    sun.setPosition(camera.getPosition());

        // need to move this to origin
        // need to move this to origin and then back a tiny bit

    while (glfwWindowShouldClose(window) == 0) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();

        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1F, 0.3F, 0.9F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        processInput(window);

        ourShader->use();

        ourShader->setUniform("light.position", glm::vec3(sun.getPosition(), 1.0F));
        // view/projection transformations
        glm::mat4 view = camera.getViewMatrix();
        ourShader->setUniform("view", view);
        ourShader->setUniform("viewPos", camera.getPosition());
        ourShader->setUniform("model", helicopterModel);
        newModel.draw(ourShader);

        ourShader->setUniform("model", backpackModel);
        model2.draw(ourShader);


        terrain.draw(view, projection, glm::vec3(sun.getPosition(), 1.0F), camera.getPosition());
        //terrain.draw(view, projection);

        view = glm::mat4(glm::mat3(camera.getViewMatrix()));
        if (updateSun) {
        sun.update(deltaTime);
        }
        skybox.draw(projection, view, sun.getPosition().y);
        sun.draw(view, projection);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    cleanup(window);
    return 0;
}

void setupGLFW() {
    if (glfwInit() == 0) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
}

auto createWindow(const int width, const int height, const char *title) -> GLFWwindow * {
    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, reshape);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetErrorCallback(errorCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    glfwMakeContextCurrent(window);

    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init("#version 410");


    return window;
}

void setupGLEW() {
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void errorCallback(const int error, const char *description) {
    std::cerr << "Error: " << error << description << std::endl;
}

void keyCallback(GLFWwindow *window, const int key, const int  /*code*/, const int action, const int  /*mod*/) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_W) {
        camera.processKeyboard(Camera::Direction::FORWARD, deltaTime);
    }
    if (key == GLFW_KEY_S) {
        camera.processKeyboard(Camera::Direction::BACKWARD, deltaTime);
    }
    if (key == GLFW_KEY_A) {
        camera.processKeyboard(Camera::Direction::LEFT, deltaTime);
    }
    if (key == GLFW_KEY_D) {
        camera.processKeyboard(Camera::Direction::RIGHT, deltaTime);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        camera.setFPS(!camera.isFPS());
    }

}

// move keys keys into function
// could store array of key presses, flip to 1 if pressed 0 otherwise
// then do update stuff based on that
// might be better in a callback or something
// prototype:

/** def input(glEnum(?) key, std::function<void()> callback, keyAction = GLFW_PRESS); */

// might be cleaner, however need to change moved thing, unless keep camera controls separate
// if in class namespace or something can keep window as global save on fuzz
void processInput(GLFWwindow *window) {
    bool moved = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (useMouse) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        useMouse = true;
      }


    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        useMouse = false;
    }


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        moved = true;
        camera.processKeyboard(Camera::Direction::FORWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        moved = true;
        camera.processKeyboard(Camera::Direction::BACKWARD, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        moved = true;
        camera.processKeyboard(Camera::Direction::LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        moved = true;
        camera.processKeyboard(Camera::Direction::RIGHT, deltaTime);
    }

    if (!moved) { camera.processKeyboard(Camera::Direction::NONE, deltaTime);
}

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        camera.setFPS(!camera.isFPS());
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        updateSun = !updateSun;
    }

}

void reshape(GLFWwindow * /*window*/, const int width, const int height) // Resize the OpenGL window
{
    projection = glm::perspective(glm::radians(camera.getZoom()),  static_cast<float>(width) /  static_cast<float>(height), 0.1F, 100.0F);
    glViewport(0, 0, width, height); // set Viewport dimensions
}

void cleanup(GLFWwindow * window) {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void mouseCallback(GLFWwindow * /*window*/, double xposIn, double yposIn) {
    const auto xpos = static_cast<float>(xposIn);
    const auto ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    const float xoffset = xpos - lastX;
    const float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow * /*window*/, const double  /*xoffset*/, const double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

