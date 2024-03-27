#include <exception>
#include <glm/ext/matrix_float3x3.hpp>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>

#include <glm/ext/matrix_transform.hpp>
#include <vector>

#include "Config.h"
#include "physics/Spline.h"

#include "App.h"
#include "physics/Constants.h"
#include "utils/Camera.h"
#include "utils/PlayerManager.h"

#include "graphics/Color.h"
#include "graphics/Model.h"
#include "graphics/Texture.h"
#include "physics/Collisions.h"
#include "utils/Objects/InfinitePlane.h"
#include "utils/Objects/Player.h"
#include "utils/Objects/Skybox.h"
#include "utils/Objects/Sun.h"
#include "utils/ShaderManager.h"
#include "graphics/ShadowBuffer.h"
#include "utils/Shader.h"

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

    ShaderManager::Add("Base", "../Assets/shaders/backpack.vert", "../Assets/shaders/backpack.frag");
    ShaderManager::Add("PostProcess", "../Assets/shaders/postProcessing.vert", "../assets/shaders/postProcessing.frag");
    ShaderManager::Add("Skybox", "../Assets/shaders/skybox.vert", "../Assets/shaders/skybox.frag");
    ShaderManager::Add("Sun", "../Assets/shaders/sun.vert", "../Assets/shaders/sun.frag");
    ShaderManager::Add("Terrain", "../Assets/shaders/infinitePlane.vert", "../Assets/shaders/infinitePlane.frag");
    ShaderManager::Add("Player", "../Assets/shaders/base.vert", "../Assets/shaders/base.frag");
    ShaderManager::Add("BoundingBox", "../Assets/shaders/boundingBox.vert", "../Assets/shaders/boundingBox.frag");
    ShaderManager::Add("Shadow", "../Assets/shaders/shadow.vert", "../Assets/shaders/shadow.frag");

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
    InfinitePlane terrain;

    Texture::Loader::setFlip(false);
    Model newModel("../Assets/objects/helecopter/chopper.obj");
    newModel.attributes.mass = 10.0F;
    Texture::Loader::setFlip(true);
    Model model2("../Assets/objects/backpack/backpack.obj");
    model2.attributes.mass = 2.0F;
    Texture::Loader::setFlip(false);

    auto orbit = std::make_shared<Player>();
    orbit->setDrawModel(false);
    orbit->getCamera().setMode(Camera::Mode::ORBIT);
    orbit->getCamera().setOrbit(glm::vec3(0.0F, 0.0F, 0.0F), 50.0F, 0.0F, 3.0F);
    orbit->getAttributes().gravityAffected = false;
    PlayerManager::Add("Orbit", orbit);

    auto free = std::make_shared<Player>();
    free->setDrawModel(true);
    free->setMode(Player::Mode::FREE);
    free->getAttributes().gravityAffected = false;
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
    fixed->getAttributes().gravityAffected = false;
    PlayerManager::Add("Fixed", fixed);

    PlayerManager::SetCurrent("Orbit");


    auto shader = ShaderManager::Get("PostProcess");
    shader->use();
    shader->setUniform("screenTexture", 0);

    shader = ShaderManager::Get("Terrain");
    shader->use();
    shader->setUniform("shadowMap", 0);

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

    // rotate by 180 degrees
    glm::mat4 helicopterModel = translate(model, newPosition);
    // helicopterModel = rotate(helicopterModel, glm::radians(180.0F), glm::vec3(0.0F, 1.0F, 0.0F));
    // scale
    // helicopterModel = scale(helicopterModel, glm::vec3(10.0F));
    // helicopterModel = rotate(helicopterModel, glm::radians(180.0F), glm::vec3(0.0F, 1.0F, 0.0F));

    newModel.transform(helicopterModel);

    model = Config::IDENTITY_MATRIX;
    newPosition = PlayerManager::GetCurrent()->getCamera().getPosition() +
                  glm::vec3(5.0F, 2.0F, 0.45F);

    const glm::mat4 backpackModel = translate(model, newPosition);

    model2.transform(backpackModel);
    // scale(glm::mat4(1.0F), glm::vec3(0.1F, 0.1F, 0.1F));

    sun.setPosition(PlayerManager::GetCurrent()->getCamera().getPosition());

    // obviously clean this up

    int numPoints = 10;
    std::vector<glm::vec3> points(numPoints);

    // generate a circle
    for (int i = 0; i < numPoints; i++) {
        float angle = 2.0F * glm::pi<float>() * i / numPoints;
        auto pos = glm::vec3(30.0F * cos(angle), 5.0F, 30.0F * sin(angle));
        points[i] = pos;
    }

    ShadowBuffer shadowBuffer = ShadowBuffer(App::view.getWidth(), App::view.getHeight());
    // ShadowBuffer shadowBuffer = ShadowBuffer(10000, 10000);

    App::view.setPipeline([&]() {
        View::clearTarget(Color::BLACK);
        auto player = PlayerManager::GetCurrent();

        const auto projectionMatrix = player->getCamera().getProjectionMatrix();
        const auto viewMatrix = player->getCamera().getViewMatrix();

        shadowBuffer.bind();

        auto lightPos = sun.getPosition();
        auto lightProjection = glm::ortho(-100.0F, 100.0F, -100.0F, 100.0F, 1.0F, 200.0F);
        auto lightView = glm::lookAt(lightPos, player->getPosition(), glm::vec3(0.0F, 1.0F, 0.0F));

        shader = ShaderManager::Get("Shadow");
        shader->use();
        terrain.draw(lightView, lightProjection, sun.getPosition(), player->getCamera().getPosition(), true);
        newModel.draw(lightView, lightProjection, true);
        model2.draw(lightView, lightProjection, true);
        PlayerManager::Draw(lightView, lightProjection, true);

        shadowBuffer.unbind();

        View::clearTarget(Color::BLACK);
        shader = ShaderManager::Get("Base");
        shader->use();
        shader->setUniform("light.position", sun.getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());

        auto texture = shadowBuffer.getTexture();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        newModel.draw(viewMatrix, projectionMatrix);
        model2.draw(viewMatrix, projectionMatrix);

        shader = ShaderManager::Get("Terrain");
        shader->use();
        shader->setUniform("light.position", sun.getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());
        shader->setUniform("projection", projectionMatrix);
        shader->setUniform("view", viewMatrix);
        shader->setUniform("shadowMap", 0);
        shader->setUniform("lightSpaceMatrix", lightProjection * lightView);

        texture = shadowBuffer.getTexture();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        terrain.draw(viewMatrix, projectionMatrix, sun.getPosition(), player->getCamera().getPosition());
        skybox.draw(projectionMatrix, viewMatrix, sun.getPosition().y);
        sun.draw(viewMatrix, projectionMatrix);

        PlayerManager::Draw(viewMatrix, projectionMatrix, false);

        //player->setDrawModel(true);
    });

    App::view.setInterface([&]() {
        if (App::paused) {
            PlayerManager::GetCurrent()->getCamera().interface();
            PlayerManager::Interface();
            PlayerManager::GetCurrent()->interface();
            App::view.optionsInterface();

            App::debugInterface();
            sun.interface();
        }

        if (App::debug) {
            PlayerManager::GetCurrent()->debug();
            ShaderManager::Interface();
        }
    });

    Physics::Spline spline(points, Physics::Spline::Type::CATMULLROM);

    try {
        App::loop([&] {
            sun.update(App::view.getDeltaTime());
            PlayerManager::GetCurrent()->update(App::view.getDeltaTime());
            newModel.update(App::view.getDeltaTime());
            model2.update(App::view.getDeltaTime());

            auto player = PlayerManager::GetCurrent();

            if (Physics::Collisions::check(terrain.getBoundingBox(),
                                           player->getBoundingBox())) {
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

            if (newModel.isColliding(model2)) {
                glm::vec3 collisionPoint = Physics::Collisions::getCollisionPoint(
                        newModel.getBoundingBox(), model2.getBoundingBox());
                Physics::Collisions::resolve(newModel.attributes, model2.attributes,
                                             collisionPoint);
            }

            if (player->getModel().isColliding(newModel)) {
                glm::vec3 collisionPoint = Physics::Collisions::getCollisionPoint(
                        player->getBoundingBox(), newModel.getBoundingBox());
                Physics::Collisions::resolve(player->getAttributes(),
                                             newModel.attributes, collisionPoint);
            }

            if (player->getModel().isColliding(model2)) {
                glm::vec3 collisionPoint = Physics::Collisions::getCollisionPoint(
                        player->getBoundingBox(), model2.getBoundingBox());
                Physics::Collisions::resolve(player->getAttributes(),
                                             model2.attributes, collisionPoint);
            }


            spline.update(App::view.getDeltaTime());
            glm::vec3 interpolatedPoint = spline.getPoint();

            auto forceNeeded =
                    newModel.attributes.calculateForce(interpolatedPoint);

            // ignore y component
            forceNeeded = glm::vec3(forceNeeded.x, 0.0F, forceNeeded.z);

            glm::vec3 torque = newModel.attributes.calculateRotation(interpolatedPoint);
            // rotate by 180 degrees
            // ignore x and z component
            torque = glm::vec3(0.0F, torque.y, 0.0F);

            //newModel.attributes.applyRotation(torque);
            //newModel.attributes.applyForce(forceNeeded);

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

    if (App::view.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS || App::view.getKey(GLFW_KEY_Q) == GLFW_PRESS) {
        App::view.close();
    }

    if (App::view.getKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
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
        PlayerManager::GetCurrent()->processKeyboard(Player::Direction::DOWN,
                                                     App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
        PlayerManager::GetCurrent()->processKeyboard(Player::Direction::UP,
                                                     App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_N) == GLFW_PRESS) {
        PlayerManager::GetCurrent()->nitro();
    }
}
