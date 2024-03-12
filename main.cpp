#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <array>
#include <string>

#include <glm/ext/matrix_transform.hpp>

#include "imgui/imgui.h"

#include "App.h"
#include "utils/Camera.h"

#include "graphics/Color.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"
#include "utils/Objects/InfinitePlane.h"
#include "utils/Objects/Skybox.h"
#include "utils/Objects/Sun.h"
#include "utils/Shader.h"

Shader *ourShader;

bool useMouse = false;

void processInput();

auto main() -> int {
    App::window("Coursework", App::DEFAULT_WIDTH, App::DEFAULT_HEIGHT);
    App::init();

    App::view.setKey(processInput);
    App::view.setMouse([&] {
        App::camera.processMouseMovement(App::view.getMouseOffsetX(), App::view.getMouseOffsetY());
    });
    App::view.setScroll([&] {
        App::camera.processMouseScroll(App::view.getScrollY());
    });
    App::view.setResize([&] {
        glViewport(0, 0, static_cast<GLsizei>(App::view.getWidth()), static_cast<GLsizei>(App::view.getHeight()));
        App::calculateProjection();
    });

    bool showMenu = false;

    App::view.setMenu([&] {
        glfwSetInputMode(App::view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        View::clearTarget(Color::BLACK);

        ImGui::Begin("Menu");
        ImGui::Text("Hello, world!");
        ImGui::Checkbox("Show Menu", &showMenu);
        ImGui::End();

        if (!showMenu) {
            App::view.setShowMenu(false);
            glfwSetInputMode(App::view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    });

    const std::array<std::string, 6> skyboxFaces{
        "../Assets/textures/skybox/right.jpg",
        "../Assets/textures/skybox/left.jpg",
        "../Assets/textures/skybox/top.jpg",
        "../Assets/textures/skybox/bottom.jpg",
        "../Assets/textures/skybox/front.jpg",
        "../Assets/textures/skybox/back.jpg",
    };

    const Skybox skybox(skyboxFaces);
    Sun sun;

    Texture::Loader::setFlip(false);
    const Model newModel("../Assets/objects/helecopter/chopper.obj");
    Texture::Loader::setFlip(true);
    const Model model2("../Assets/objects/backpack/backpack.obj");
    Texture::Loader::setFlip(false);

    App::camera.setOrbit(glm::vec3(0.0F, 0.0F, 0.0F), 10.0F, 5.0F, 5.0F);
    App::camera.setMode(Camera::Mode::ORBIT);


    const InfinitePlane terrain;
    ourShader = new Shader("../Assets/shaders/backpack.vert", "../Assets/shaders/backpack.frag");

    ourShader->use();
    ourShader->setUniform("projection", App::projection);
    ourShader->setUniform("light.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
    ourShader->setUniform("light.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
    ourShader->setUniform("light.specular", glm::vec3(1.0F, 1.0F, 1.0F));
    // material properties
    ourShader->setUniform("material.specular", glm::vec3(0.5F, 0.5F, 0.5F));
    ourShader->setUniform("material.shininess", 64.0F);

    auto model = glm::mat4(1.0F);
    glm::vec3 newPosition = App::camera.getPosition() + glm::vec3(0.0F, -0.15F, 0.45F);
    const glm::mat4 helicopterModel = translate(model, newPosition) * scale(
                                          glm::mat4(1.0F), glm::vec3(0.3F, 0.3F, 0.3F));

    ourShader->setUniform("model", helicopterModel);

    model = glm::mat4(1.0F);
    newPosition = App::camera.getPosition() + glm::vec3(5.0F, -0.15F, 0.45F);
    const glm::mat4 backpackModel = translate(model, newPosition) * scale(glm::mat4(1.0F), glm::vec3(0.1F, 0.1F, 0.1F));

    sun.setPosition(App::camera.getPosition());

    App::view.setPipeline([&]() {
        View::clearTarget(Color::BLACK);

        ourShader->use();

        ourShader->setUniform("light.position", glm::vec3(sun.getPosition(), 1.0F));
        // view/projection transformations
        glm::mat4 view = App::camera.getViewMatrix();
        ourShader->setUniform("view", view);
        ourShader->setUniform("viewPos", App::camera.getPosition());
        ourShader->setUniform("model", helicopterModel);
        // newModel.draw(ourShader);

        ourShader->setUniform("model", backpackModel);
        // model2.draw(ourShader);

        terrain.draw(view, App::projection, glm::vec3(sun.getPosition(), 1.0F), App::camera.getPosition());

        view = glm::mat4(glm::mat3(App::camera.getViewMatrix()));
        skybox.draw(App::projection, view, sun.getPosition().y);
        sun.draw(view, App::projection);
    });

    App::view.setInterface([&]() {
        App::camera.modeInterface();
    });

    App::loop([&] {
        sun.update(App::view.getDeltaTime());
        App::camera.circleOrbit(App::view.getDeltaTime());
    });
    App::quit();
}

// move keys keys into function
// could store array of key presses, flip to 1 if pressed 0 otherwise
// then do update stuff based on that
// might be better in a callback or something
// prototype:

/** def input(glEnum(?) key, std::function<void()> callback, keyAction = GLFW_PRESS); */

// might be cleaner, however need to change moved thing, unless keep App::camera controls separate
// if in class namespace or something can keep window as global save on fuzz
void processInput() {
    bool moved = false;

    if (glfwGetKey(App::view.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (useMouse) {
            glfwSetWindowShouldClose(App::view.getWindow(), GLFW_TRUE);
        }

        glfwSetInputMode(App::view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        useMouse = true;
    }

    if (glfwGetKey(App::view.getWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
        glfwSetInputMode(App::view.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        useMouse = false;
    }

    if (glfwGetKey(App::view.getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
        moved = true;
        App::camera.processKeyboard(Camera::Direction::FORWARD, App::view.getDeltaTime());
    }

    if (glfwGetKey(App::view.getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
        moved = true;
        App::camera.processKeyboard(Camera::Direction::BACKWARD, App::view.getDeltaTime());
    }

    if (glfwGetKey(App::view.getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
        moved = true;
        App::camera.processKeyboard(Camera::Direction::LEFT, App::view.getDeltaTime());
    }

    if (glfwGetKey(App::view.getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
        moved = true;
        App::camera.processKeyboard(Camera::Direction::RIGHT, App::view.getDeltaTime());
    }

    if (!moved) {
        App::camera.processKeyboard(Camera::Direction::NONE, App::view.getDeltaTime());
    }

    if (glfwGetKey(App::view.getWindow(), GLFW_KEY_F) == GLFW_PRESS) {
        switch (App::camera.getMode()) {
            case Camera::Mode::ORBIT:
                App::camera.setMode(Camera::Mode::FREE);
                break;
            case Camera::Mode::FREE:
                App::camera.setMode(Camera::Mode::FPS);
                break;
            case Camera::Mode::FPS:
                App::camera.setMode(Camera::Mode::ORBIT);
                break;
            default:
                break;
        }
    }

    if (glfwGetKey(App::view.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
        App::paused = !App::paused;
    }
}
