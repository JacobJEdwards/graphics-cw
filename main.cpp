#include <exception>
#include <glm/ext/matrix_float3x3.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/spline.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>

#include <glm/ext/matrix_transform.hpp>
#include <vector>

#include "Config.h"

#include "App.h"
#include "physics/Constants.h"
#include "utils/Camera.h"
#include "utils/PlayerManager.h"

#include "graphics/Color.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"
#include "physics/Collisions.h"
#include "shapes/Plane.h"
#include "utils/Objects/InfinitePlane.h"
#include "utils/Objects/Player.h"
#include "utils/Objects/Skybox.h"
#include "utils/Objects/Sun.h"
#include "utils/Shader.h"
#include "utils/ShaderManager.h"

void processInput();

auto main() -> int {
    App::window("Coursework", Config::DEFAULT_WIDTH, Config::DEFAULT_HEIGHT);
    App::init();

    App::view.setKey(processInput);
    App::view.setMouse([&] {
        if (!App::paused) {
            PlayerManager::GetCurrent()->getCamera().processMouseMovement(
                    App::view.getMouseOffsetX(), App::view.getMouseOffsetY());
        }
    });

    App::view.setScroll([&] {
        PlayerManager::GetCurrent()->getCamera().processMouseScroll(
                App::view.getScrollY());
    });

    App::view.setResize([&] {
        PlayerManager::SetAspect(static_cast<float>(App::view.getWidth()) /
                                 static_cast<float>(App::view.getHeight()));
        glViewport(0, 0, static_cast<GLsizei>(App::view.getWidth()),
                   static_cast<GLsizei>(App::view.getHeight()));
    });

    const std::array<std::string, 6> skyboxFaces{
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
    Model newModel("../Assets/objects/helecopter/chopper.obj");
    newModel.attributes.mass = 50.0F;
    Texture::Loader::setFlip(true);
    Model model2("../Assets/objects/backpack/backpack.obj");
    model2.attributes.mass = 0.1F;
    Texture::Loader::setFlip(false);

    auto orbit = std::make_shared<Player>();
    orbit->setDrawModel(false);
    orbit->getCamera().setMode(Camera::Mode::ORBIT);
    orbit->getCamera().setOrbit(glm::vec3(0.0F, 0.0F, 0.0F), 50.0F, 0.0F, 3.0F);
    PlayerManager::Add("Orbit", orbit);

    auto free = std::make_shared<Player>();
    free->setDrawModel(true);
    free->setMode(Player::Mode::FREE);
    PlayerManager::Add("Free", free);

    auto fps = std::make_shared<Player>();
    fps->setDrawModel(true);
    fps->setMode(Player::Mode::FPS);
    PlayerManager::Add("FPS", fps);

    auto fixed = std::make_shared<Player>();
    fixed->setDrawModel(true);
    fixed->getCamera().setFixed(glm::vec3(0.0F, 0.0F, 0.0F),
                                glm::vec3(4.0F, 3.0F, 6.0F));
    fixed->setMode(Player::Mode::FIXED);
    PlayerManager::Add("Fixed", fixed);

    PlayerManager::SetCurrent("Orbit");

    InfinitePlane terrain;

    Plane postProcessingPlane;

    ShaderManager::Add("Base", "../Assets/shaders/backpack.vert", "../Assets/shaders/backpack.frag");
    ShaderManager::Add("PostProcess", "../Assets/shaders/postProcessing.vert", "../assets/shaders/postProcessing.frag");

    auto shader = ShaderManager::Get("PostProcess");

    shader->use();
    shader->setUniform("screenTexture", 0);

    shader = ShaderManager::Get("Base");
    newModel.setShader(shader);
    model2.setShader(shader);

    shader->use();

    shader->setUniform("light.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
    shader->setUniform("light.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
    shader->setUniform("light.specular", glm::vec3(1.0F, 1.0F, 1.0F));
    // material properties
    shader->setUniform("material.specular", glm::vec3(0.5F, 0.5F, 0.5F));
    shader->setUniform("material.shininess", 64.0F);

    auto model = glm::mat4(1.0F);

    glm::vec3 newPosition = PlayerManager::GetCurrent()->getCamera().getPosition() +
                            glm::vec3(20.0F, 10.0F, 8.0F);

    const glm::mat4 helicopterModel = translate(model, newPosition);

    newModel.transform(helicopterModel);

    model = Config::IDENTITY_MATRIX;
    newPosition = PlayerManager::GetCurrent()->getCamera().getPosition() +
                  glm::vec3(5.0F, 2.0F, 0.45F);

    const glm::mat4 backpackModel = translate(model, newPosition);

    model2.transform(backpackModel);
    // scale(glm::mat4(1.0F), glm::vec3(0.1F, 0.1F, 0.1F));

    sun.setPosition(PlayerManager::GetCurrent()->getCamera().getPosition());

    // obviously clean this up

    int numPoints = 100;
    std::vector<glm::vec3> points(numPoints);

    // generate a circle
    for (int i = 0; i < numPoints; i++) {
        float angle = 2.0F * glm::pi<float>() * i / numPoints;
        auto pos = glm::vec3(20.0F * cos(angle), 5.0F, 20.0F * sin(angle));
        points[i] = pos;
    }

    int p0Index = 0;
    int p1Index = 1;
    int p2Index = 2;
    int p3Index = 3;

    App::view.setPipeline([&]() {
        View::clearTarget(Color::BLACK);
        auto player = PlayerManager::GetCurrent();

        const auto projectionMatrix = player->getCamera().getProjectionMatrix();

        const auto viewMatrix = player->getCamera().getViewMatrix();

        shader->use();

        shader->setUniform("projection", projectionMatrix);

        shader->setUniform("light.position", glm::vec3(sun.getPosition(), 1.0F));
        shader->setUniform("view", viewMatrix);

        shader->setUniform("viewPos", player->getCamera().getPosition());

        newModel.draw(viewMatrix, projectionMatrix);
        model2.draw(viewMatrix, projectionMatrix);
        terrain.draw(viewMatrix, projectionMatrix,
                     glm::vec3(sun.getPosition(), 1.0F),
                     player->getCamera().getPosition());

        skybox.draw(projectionMatrix, viewMatrix, sun.getPosition().y);
        sun.draw(viewMatrix, projectionMatrix);

        PlayerManager::Draw(viewMatrix, projectionMatrix);
    });

    App::view.setInterface([&]() {
        if (App::paused) {
            PlayerManager::GetCurrent()->getCamera().interface();
            PlayerManager::Interface();
            PlayerManager::GetCurrent()->interface();
            App::view.optionsInterface();

            App::debugInterface();
        }

        if (App::debug) {
            PlayerManager::GetCurrent()->debug();
        }
    });

    float t = 0.0F; // spline parameter
    try {
        App::loop([&] {
            auto player = PlayerManager::GetCurrent();
            if (Physics::Collisions::check(player->getBoundingBox(),
                                           terrain.getBoundingBox())) {
                Physics::Collisions::resolve(player->getAttributes(),
                                             Physics::FLOOR_NORMAL);
                player->getAttributes().isGrounded = true;

            } else {
                player->getAttributes().isGrounded = false;
            }

            if (Physics::Collisions::check(newModel.getBoundingBox(),
                                           terrain.getBoundingBox())) {
                Physics::Collisions::resolve(newModel.attributes,
                                             Physics::FLOOR_NORMAL);
                newModel.attributes.isGrounded = true;
            } else {
                newModel.attributes.isGrounded = false;
            }

            if (Physics::Collisions::check(model2.getBoundingBox(),
                                           terrain.getBoundingBox())) {
                Physics::Collisions::resolve(model2.attributes, Physics::FLOOR_NORMAL);
                model2.attributes.isGrounded = true;
            } else {
                model2.attributes.isGrounded = false;
            }

            if (Physics::Collisions::check(newModel.getBoundingBox(),
                                           model2.getBoundingBox())) {
                Physics::Collisions::resolve(newModel.attributes, model2.attributes);
            }

            if (Physics::Collisions::check(player->getBoundingBox(),
                                           newModel.getBoundingBox())) {
                Physics::Collisions::resolve(player->getAttributes(),
                                             newModel.attributes);
            }

            if (Physics::Collisions::check(player->getBoundingBox(),
                                           model2.getBoundingBox())) {
                Physics::Collisions::resolve(player->getAttributes(),
                                             model2.attributes);
            }

            if (p3Index == numPoints) {
                p3Index = 0;
            }

            if (p2Index == numPoints) {
                p2Index = 0;
            }

            if (p1Index == numPoints) {
                p1Index = 0;
            }

            if (p0Index == numPoints) {
                p0Index = 0;
            }

            glm::vec3 interpolatedPoint =
                    glm::catmullRom(points[p0Index], points[p1Index], points[p2Index],
                                    points[p3Index], t);

            auto forceNeeded =
                    newModel.attributes.calculateForce(interpolatedPoint, 0.1F);

            forceNeeded = glm::vec3(forceNeeded.x, 0.0F, forceNeeded.z);

            newModel.attributes.applyForce(forceNeeded);

            t += 0.1F;

            if (t > 1.0F) {
                t = 0.0F;
                p0Index++;
                p1Index++;
                p2Index++;
                p3Index++;
            }

            sun.update(App::view.getDeltaTime());

            PlayerManager::GetCurrent()->update(App::view.getDeltaTime());
            newModel.update(App::view.getDeltaTime());
            model2.update(App::view.getDeltaTime());
        });
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    App::quit();
}

// move keys keys into function
// could store array of key presses, flip to 1 if pressed 0 otherwise
// then do update stuff based on that
// might be better in a callback or something
// prototype:

/** def input(glEnum(?) key, std::function<void()> callback, keyAction =
 * GLFW_PRESS); */

// might be cleaner, however need to change moved thing, unless keep App::camera
// controls separate if in class namespace or something can keep window as
// global save on fuzz
void processInput() {
    bool moved = false;

    if (App::view.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        App::view.close();
    }

    if (App::view.getKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
        App::setPaused(!App::paused);
    }

    if (App::paused &&
        PlayerManager::GetCurrent()->getCamera().getMode() != Camera::Mode::ORBIT) {
        return;
    }

    if (App::view.getKey(GLFW_KEY_W) == GLFW_PRESS) {
        moved = true;
        PlayerManager::GetCurrent()->processKeyboard(Player::Direction::FORWARD,
                                                     App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_S) == GLFW_PRESS) {
        moved = true;
        PlayerManager::GetCurrent()->processKeyboard(Player::Direction::BACKWARD,
                                                     App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_A) == GLFW_PRESS) {
        moved = true;
        PlayerManager::GetCurrent()->processKeyboard(Player::Direction::LEFT,
                                                     App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_D) == GLFW_PRESS) {
        moved = true;
        PlayerManager::GetCurrent()->processKeyboard(Player::Direction::RIGHT,
                                                     App::view.getDeltaTime());
    }

    if (!moved) {
        PlayerManager::GetCurrent()->processKeyboard(Player::Direction::NONE,
                                                     App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        PlayerManager::GetCurrent()->processKeyboard(Player::Direction::UP,
                                                     App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        PlayerManager::GetCurrent()->processKeyboard(Player::Direction::DOWN,
                                                     App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_N) == GLFW_PRESS) {
        PlayerManager::GetCurrent()->nitro();
    }

    if (App::view.getKey(GLFW_KEY_J) == GLFW_PRESS) {
        if (PlayerManager::GetCurrent()->getAttributes().isGrounded) {
            PlayerManager::GetCurrent()->jump();
        }
    }
}
