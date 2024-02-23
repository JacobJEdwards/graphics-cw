#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Graphics/Shader.h"
#include "Game/Camera.h"
#include "Graphics/Texture.h"
#include "Graphics/Model.h"

constexpr unsigned int SCR_WIDTH = 800;
constexpr unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0F, 0.0F, 3.0F));

bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0F;
float lastY = SCR_HEIGHT / 2.0F;

float deltaTime = 0.0F;
float lastFrame = 0.0F;

glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()),static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1F, 100.0F);

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

auto setupSkybox(const Shader &shader) -> GLuint;

auto getSkyboxTexture() -> GLuint;

void drawSkybox(const Shader &shader, GLuint VAO, GLuint texture);

auto main() -> int {
    setupGLFW();
    GLFWwindow *window = createWindow(SCR_WIDTH, SCR_HEIGHT, "Coursework");
    setupGLEW();

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    const Shader skyboxShader("../Assets/shaders/skybox.vert", "../Assets/shaders/skybox.frag");
    const Shader ourShader("../Assets/shaders/backpack.vert", "../Assets/shaders/backpack.frag");

    const GLuint skyboxVAO = setupSkybox(skyboxShader);
    const GLuint skyboxTexture = getSkyboxTexture();


    Texture::Loader::setFlip(true);
    //const Model ourModel("../Assets/backpack/backpack.obj");

    const Model newModel("../Assets/objects/backpack/backpack.obj");

    projection = glm::perspective(glm::radians(camera.getZoom()), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1F, 100.0F);


    while (glfwWindowShouldClose(window) == 0) {
        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1F, 0.1F, 0.1F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        // view/projection transformations
        glm::mat4 view = camera.getViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        auto model = glm::mat4(1.0f);
        model = translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        newModel.draw(ourShader);

        drawSkybox(skyboxShader, skyboxVAO, skyboxVAO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
    glfwSetErrorCallback(errorCallback);

    glfwMakeContextCurrent(window);
    return window;
}

void drawSkybox(const Shader &shader, const GLuint VAO, const GLuint texture) {
        glDepthFunc(GL_LEQUAL);
        shader.use();
        auto view = glm::mat4(glm::mat3(camera.getViewMatrix()));
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

}

auto setupSkybox(const Shader &shader) -> GLuint {
    constexpr GLfloat skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    GLuint skyboxVAO;
    GLuint skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    shader.use();
    shader.setInt("skybox", 0);

    return skyboxVAO;
}

auto getSkyboxTexture() -> GLuint {
    const std::vector<std::string> skyboxFaces {
        "../Assets/textures/skybox/right.jpg",
        "../Assets/textures/skybox/left.jpg",
        "../Assets/textures/skybox/top.jpg",
        "../Assets/textures/skybox/bottom.jpg",
        "../Assets/textures/skybox/front.jpg",
        "../Assets/textures/skybox/back.jpg",
    };

    return Texture::Loader::loadCubemap(skyboxFaces);
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
    projection = glm::perspective(glm::radians(camera.getZoom()),  static_cast<float>(width) /  static_cast<float>(height), 0.1F, 100.0F);
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

