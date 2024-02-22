#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Graphics/Shader.h"
#include "Game/Camera.h"

constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0F, 0.0F, 3.0F));

bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0F;
float lastY = SCR_HEIGHT / 2.0F;

float deltaTime = 0.0F;
float lastFrame = 0.0F;

glm::mat4 projection = glm::perspective(glm::radians(camera.zoom),static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1F, 100.0F);

glm::vec3 lightPos(1.2F, 1.0F, 2.0F);

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
    glfwSetErrorCallback(errorCallback);
    glEnable(GL_DEPTH_TEST);

    Shader shader("../Assets/basic.vert", "../Assets/basic.frag");
    Shader lightingShader("../Assets/lighting.vert", "../Assets/lighting.frag");


    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0F, 0.0F, -1.0F,
        0.5f, -0.5f, -0.5f, 0.0F, 0.0F, -1.0F,
        0.5f, 0.5f, -0.5f, 0.0F, 0.0F, -1.0F,
        0.5f, 0.5f, -0.5f, 0.0F, 0.0F, -1.0F,
        -0.5f, 0.5f, -0.5f, 0.0F, 0.0F, -1.0F,
        -0.5f, -0.5f, -0.5f, 0.0F, 0.0F, -1.0F,

        -0.5f, -0.5f, 0.5f, 0.0F, 0.0F, 1.0F,
        0.5f, -0.5f, 0.5f, 0.0F, 0.0F, 1.0F,
        0.5f, 0.5f, 0.5f, 0.0F, 0.0F, 1.0F,
        0.5f, 0.5f, 0.5f, 0.0F, 0.0F, 1.0F,
        -0.5f, 0.5f, 0.5f, 0.0F, 0.0F, 1.0F,
        -0.5f, -0.5f, 0.5f, 0.0F, 0.0F, 1.0F,

        -0.5f, 0.5f, 0.5f, -1.0F, 0.0F, 0.0F,
        -0.5f, 0.5f, -0.5f, -1.0F, 0.0F, 0.0F,
        -0.5f, -0.5f, -0.5f, -1.0F, 0.0F, 0.0F,
        -0.5f, -0.5f, -0.5f, -1.0F, 0.0F, 0.0F,
        -0.5f, -0.5f, 0.5f, -1.0F, 0.0F, 0.0F,
        -0.5f, 0.5f, 0.5f, -1.0F, 0.0F, 0.0F,

        0.5f, 0.5f, 0.5f, 1.0F, 0.0F, 0.0F,
        0.5f, 0.5f, -0.5f, 1.0F, 0.0F, 0.0F,
        0.5f, -0.5f, -0.5f, 1.0F, 0.0F, 0.0F,
        0.5f, -0.5f, -0.5f, 1.0F, 0.0F, 0.0F,
        0.5f, -0.5f, 0.5f, 1.0F, 0.0F, 0.0F,
        0.5f, 0.5f, 0.5f, 1.0F, 0.0F, 0.0F,

        -0.5f, -0.5f, -0.5f, 0.0F, -1.0F, 0.0F,
        0.5f, -0.5f, -0.5f, 0.0F, -1.0F, 0.0F,
        0.5f, -0.5f, 0.5f, 0.0F, -1.0F, 0.0F,
        0.5f, -0.5f, 0.5f, 0.0F, -1.0F, 0.0F,
        -0.5f, -0.5f, 0.5f, 0.0F, -1.0F, 0.0F,
        -0.5f, -0.5f, -0.5f, 0.0F, -1.0F, 0.0F,

        -0.5f, 0.5f, -0.5f, 0.0F, 1.0F, 0.0F,
        0.5f, 0.5f, -0.5f, 0.0F, 1.0F, 0.0F,
        0.5f, 0.5f, 0.5f, 0.0F, 1.0F, 0.0F,
        0.5f, 0.5f, 0.5f, 0.0F, 1.0F, 0.0F,
        -0.5f, 0.5f, 0.5f, 0.0F, 1.0F, 0.0F,
        -0.5f, 0.5f, -0.5f, 0.0F, 1.0F, 0.0F
    };

    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0F, 0.0F, 0.0F),
        glm::vec3(2.0F, 5.0F, -15.0F),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0F, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0F, -7.5f),
        glm::vec3(1.3f, -2.0F, -2.5f),
        glm::vec3(1.5f, 2.0F, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0F, -1.5f)
    };


    GLuint VBO;
    GLuint cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), static_cast<float *>(vertices), GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    GLuint lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    projection = glm::perspective(glm::radians(camera.zoom), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1F, 100.0F);

    while (glfwWindowShouldClose(window) == 0) {
        auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


        glClearColor(0.1f, 0.1f, 0.1f, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);

        shader.use();

        lightPos.x = 1.0F + sin(glfwGetTime()) * 2.0F;
        lightPos.y = sin(glfwGetTime() / 2.0F) * 1.0F;
        lightPos.z = cos(glfwGetTime()) * 2.0F;

        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0F);
        lightColor.y = sin(glfwGetTime() * 0.7F);
        lightColor.z = sin(glfwGetTime() * 1.3F);

        shader.setVec3("light.position", lightPos);
        shader.setVec3("viewPos", camera.position);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5F); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2F); // low influence

        shader.setVec3("light.ambient", ambientColor);
        shader.setVec3("light.diffuse", diffuseColor);
        shader.setVec3("light.specular", 1.0F, 1.0F, 1.0F);

        shader.setVec3("material.specular", 0.5F, 0.5F, 0.5F);
        shader.setFloat("material.shininess", 32.0F);
        shader.setVec3("material.ambient", 1.0F, 0.5F, 0.31F);
        shader.setVec3("material.diffuse", 1.0F, 0.5F, 0.31F);


        shader.setMat4("projection", projection);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.

        auto view = camera.getViewMatrix();
        shader.setMat4("view", view);

        auto model = glm::mat4(1.0F);
        shader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        model = glm::mat4(1.0F);
        model = translate(model, lightPos);
        model = scale(model, glm::vec3(0.2F));
        lightingShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);

    cleanup(window);
    return 0;
}

void setupGLFW() {
    if (glfwInit() == 0) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

    glfwMakeContextCurrent(window);
    return window;
}

void setupGLEW() {
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void errorCallback(int error, const char *description) {
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

void processInput(GLFWwindow *window) {
    bool moved = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        moved = true;
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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
}

void reshape(GLFWwindow * /*window*/, const int width, const int height) // Resize the OpenGL window
{
    projection = glm::perspective(glm::radians(camera.zoom),  static_cast<float>(width) /  static_cast<float>(height), 0.1F, 100.0F);
    glViewport(0, 0, width, height); // set Viewport dimensions
}

void cleanup(GLFWwindow * /*window*/) {
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

