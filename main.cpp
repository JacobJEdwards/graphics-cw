#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <memory>

#include <glm/ext/matrix_transform.hpp>
#include <vector>

#include "Config.h"
#include "physics/Spline.h"

#include "App.h"
#include "utils/Camera.h"
#include "utils/PlayerManager.h"

#include "graphics/Color.h"
#include "graphics/Texture.h"
#include "physics/Collisions.h"
#include "utils/Objects/InfinitePlane.h"
#include "utils/Objects/Player.h"
#include "utils/Objects/Skybox.h"
#include "utils/ShaderManager.h"
#include "graphics/ShadowBuffer.h"
#include "Entity.h"
#include "utils/Objects/ProceduralTerrain.h"
#include "graphics/Particle.h"
#include "utils/Objects/BumperCar.h"
#include <random>

void processInput();

void setupApp();

void setupShaders();

void setupPlayers();

auto main() -> int {
    setupApp();
    setupShaders();
    setupPlayers();

    Skybox skybox;
    ProceduralTerrain newTerrain;
    InfinitePlane terrain;

    Entity model2("../Assets/objects/bumpercar2/bumper-car.obj");
    model2.getAttributes().mass = 2.0F;
    Texture::Loader::setFlip(false);

    const int numPoints = 10;
    std::vector<glm::vec3> points(numPoints);

    // generate a circle
    for (int i = 0; i < numPoints; i++) {
        const float angle = 2.0F * glm::pi<float>() * i / numPoints;
        auto pos = glm::vec3(30.0F * std::cos(angle), 5.0F, 30.0F * std::sin(angle));
        points[i] = pos;
    }

    Texture::Loader::setFlip(true);
    BumperCar bumperCar(points, 1.0F);

    auto shader = ShaderManager::Get("Base");
    model2.setShader(shader);
    bumperCar.setShader(shader);
    PlayerManager::Get("Orbit")->setShader(shader);
    PlayerManager::Get("Free")->setShader(shader);
    PlayerManager::Get("FPS")->setShader(shader);
    PlayerManager::Get("Fixed")->setShader(shader);

    auto model = Config::IDENTITY_MATRIX;
    glm::vec3 newPosition = PlayerManager::GetCurrent()->getCamera().getPosition() +
                            glm::vec3(20.0F, 10.0F, 8.0F);
    model = translate(model, newPosition);
    model = scale(model, glm::vec3(4.0F));
    // make forward vector point to the right
    bumperCar.transform(model);

    model = Config::IDENTITY_MATRIX;
    newPosition = PlayerManager::GetCurrent()->getCamera().getPosition() +
                  glm::vec3(5.0F, 2.0F, 0.45F);
    model = translate(model, newPosition);
    model = scale(model, glm::vec3(4.0F));
    model2.transform(model);

    model = Config::IDENTITY_MATRIX;

    ParticleSystem particleSystem;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-0.5, 0.5);

    ShadowBuffer shadowBuffer = ShadowBuffer(App::view.getWidth(), App::view.getHeight());
    App::view.setPipeline([&]() {
        View::clearTarget(Color::BLACK);
        auto player = PlayerManager::GetCurrent();

        const auto projectionMatrix = player->getCamera().getProjectionMatrix();
        const auto viewMatrix = player->getCamera().getViewMatrix();

        shadowBuffer.bind();

        auto lightPos = skybox.getSun().getPosition();
        auto lightProjection = glm::ortho(-100.0F, 100.0F, -100.0F, 100.0F, 1.0F, 200.0F);
        auto lightView = glm::lookAt(lightPos, player->getPosition(), glm::vec3(0.0F, 1.0F, 0.0F));

        shader = ShaderManager::Get("Shadow");
        shader->use();
        shader->setUniform("view", lightView);
        shader->setUniform("projection", lightProjection);

        terrain.draw(shader);
        bumperCar.draw(shader);
        model2.draw(shader);
        newTerrain.draw(shader);

        PlayerManager::Draw(shader);

        shadowBuffer.unbind();

        View::clearTarget(Color::BLACK);
        shader = ShaderManager::Get("Base");
        shader->use();
        shader->setUniform("light.position", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());

        auto texture = shadowBuffer.getTexture();

        bumperCar.draw(viewMatrix, projectionMatrix);
        model2.draw(viewMatrix, projectionMatrix);

        particleSystem.draw(viewMatrix, projectionMatrix);

        shader = terrain.getShader();

        shader->use();
        shader->setUniform("light.position", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());
        shader->setUniform("projection", projectionMatrix);
        shader->setUniform("view", viewMatrix);
        shader->setUniform("shadowMap", 0);
        shader->setUniform("lightSpaceMatrix", lightProjection * lightView);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        shader = ShaderManager::Get("PostProcess");
        shader->use();
        shader->setUniform("screenTexture", 0);
        shader->setUniform("sunPosition", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());

        PlayerManager::Draw(viewMatrix, projectionMatrix);
        particleSystem.draw(viewMatrix, projectionMatrix);

        shader = newTerrain.getShader();
        shader->use();
        shader->setUniform("light.position", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());
        shader->setUniform("projection", projectionMatrix);
        shader->setUniform("view", viewMatrix);
        shader->setUniform("lightSpaceMatrix", lightProjection * lightView);
        shader->setUniform("shadowMap", 0);
        shader->setUniform("model", model);

        newTerrain.draw(viewMatrix, projectionMatrix);
        skybox.draw(viewMatrix, projectionMatrix);
    });

    App::view.setInterface([&]() {
        if (App::paused) {
            PlayerManager::GetCurrent()->getCamera().interface();
            PlayerManager::Interface();
            PlayerManager::GetCurrent()->interface();
            App::view.optionsInterface();
            BumperCar::Interface();

            App::debugInterface();
            skybox.getSun().interface();
        }

        if (App::debug) {
            PlayerManager::GetCurrent()->debug();
            ShaderManager::Interface();
        }
    });

    try {
        App::loop([&] {
            auto player = PlayerManager::GetCurrent();

            player->update(App::view.getDeltaTime());
            bumperCar.update(App::view.getDeltaTime());
            model2.update(App::view.getDeltaTime());
            skybox.update(App::view.getDeltaTime());
            particleSystem.update(App::view.getDeltaTime());

            for (int i = 0; i < 100; i++) {
                Particle particle;
                particle.position = bumperCar.attributes.position;
                particle.position.x += dis(gen) * 2.0F;
                particle.position.y += dis(gen);
                particle.position.z += dis(gen) * 2.0F;

                particle.velocity = bumperCar.attributes.velocity * 0.1F;
                particle.velocity.x += dis(gen) * 2.0F;
                particle.velocity.y += dis(gen);
                particle.velocity.z += dis(gen) * 2.0F;

                particle.life = 1.0F;
                particle.color = glm::vec4(0.0F, 0.0F, 0.0F, 0.8F);

                particle.color.x += dis(gen);
                particle.color.y += dis(gen);
                particle.color.z += dis(gen);

                particleSystem.add(particle);
            }

            if (Physics::Collisions::check(bumperCar.getBoundingBox(), model2.getBoundingBox())) {
                const glm::vec3 collisionPoint = Physics::Collisions::getCollisionPoint(
                        bumperCar.getBoundingBox(), model2.getBoundingBox());
                Physics::Collisions::resolve(bumperCar.attributes, model2.attributes,
                                             collisionPoint);
            }

            if (Physics::Collisions::check(player->getBoundingBox(), bumperCar.getBoundingBox())) {
                const glm::vec3 collisionPoint = Physics::Collisions::getCollisionPoint(
                        player->getBoundingBox(), bumperCar.getBoundingBox());
                Physics::Collisions::resolve(player->getAttributes(),
                                             bumperCar.attributes, collisionPoint);

                if (glm::length(player->getAttributes().force) > 0.1F) {
                    App::view.blurScreen();
                }
            }

            if (Physics::Collisions::check(player->getBoundingBox(), model2.getBoundingBox())) {
                const glm::vec3 collisionPoint = Physics::Collisions::getCollisionPoint(
                        player->getBoundingBox(), model2.getBoundingBox());
                Physics::Collisions::resolve(player->getAttributes(),
                                             model2.attributes, collisionPoint);
            }


            auto position = player->getPosition();
            if (newTerrain.intersectRay(position)) {
                Physics::Collisions::resolve(player->getAttributes(),
                                             newTerrain.getTerrainNormal(position.x, position.z));
                player->attributes.isGrounded = true;
            } else {
                player->attributes.isGrounded = false;
            }

            position = bumperCar.attributes.position;

            if (newTerrain.intersectRay(position)) {
                Physics::Collisions::resolve(bumperCar.attributes,
                                             newTerrain.getTerrainNormal(position.x, position.z));
                bumperCar.attributes.isGrounded = true;
            } else {
                bumperCar.attributes.isGrounded = false;
            }

            position = model2.attributes.position;
            if (newTerrain.intersectRay(position)) {
                Physics::Collisions::resolve(model2.attributes,
                                             newTerrain.getTerrainNormal(position.x, position.z));
                model2.attributes.isGrounded = true;
            } else {
                model2.attributes.isGrounded = false;
            }


        });
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    App::quit();
}

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

void setupApp() {
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
}

void setupShaders() {
    ShaderManager::Add("Simple", "../Assets/shaders/base.vert", "../Assets/shaders/base.frag");
    ShaderManager::Add("Base", "../Assets/shaders/backpack.vert", "../Assets/shaders/backpack.frag");
    ShaderManager::Add("PostProcess", "../Assets/shaders/postProcessing.vert", "../assets/shaders/postProcessing.frag");
    ShaderManager::Add("Skybox", "../Assets/shaders/skybox.vert", "../Assets/shaders/skybox.frag");
    ShaderManager::Add("Sky", "../Assets/shaders/Sky.vert", "../Assets/shaders/Sky.frag");
    ShaderManager::Add("Sun", "../Assets/shaders/sun.vert", "../Assets/shaders/sun.frag");
    ShaderManager::Add("Terrain", "../Assets/shaders/infinitePlane.vert", "../Assets/shaders/infinitePlane.frag");
    ShaderManager::Add("Player", "../Assets/shaders/base.vert", "../Assets/shaders/base.frag");
    ShaderManager::Add("BoundingBox", "../Assets/shaders/boundingBox.vert", "../Assets/shaders/boundingBox.frag");
    ShaderManager::Add("Shadow", "../Assets/shaders/shadow.vert", "../Assets/shaders/shadow.frag");
    ShaderManager::Add("Particle", "../Assets/shaders/particle.vert", "../Assets/shaders/particle.frag");

    auto shader = ShaderManager::Get("PostProcess");
    shader->use();
    shader->setUniform("screenTexture", 0);

    shader = ShaderManager::Get("Terrain");
    shader->use();
    shader->setUniform("shadowMap", 0);

    shader = ShaderManager::Get("Base");
    shader->use();

    shader->setUniform("light.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
    shader->setUniform("light.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
    shader->setUniform("light.specular", glm::vec3(1.0F, 1.0F, 1.0F));
    // material properties
    shader->setUniform("material.specular", glm::vec3(0.5F, 0.5F, 0.5F));
    shader->setUniform("material.shininess", 64.0F);
}

void setupPlayers() {
    auto orbit = std::make_shared<Player>();
    orbit->shouldDraw(false);
    orbit->getCamera().setMode(Camera::Mode::ORBIT);
    orbit->getCamera().setOrbit(glm::vec3(0.0F, 0.0F, 0.0F), 50.0F, 0.0F, 3.0F);
    orbit->getAttributes().gravityAffected = false;
    PlayerManager::Add("Orbit", orbit);

    auto free = std::make_shared<Player>();
    free->shouldDraw(true);
    free->setMode(Player::Mode::FREE);
    free->getAttributes().gravityAffected = false;
    PlayerManager::Add("Free", free);

    auto fps = std::make_shared<Player>();
    fps->shouldDraw(true);
    fps->setMode(Player::Mode::FPS);
    PlayerManager::Add("FPS", fps);

    auto fixed = std::make_shared<Player>();
    fixed->shouldDraw(true);
    fixed->getCamera().setFixed(glm::vec3(0.0F, 0.0F, 0.0F),
                                glm::vec3(4.0F, 3.0F, 6.0F));
    fixed->setMode(Player::Mode::FIXED);
    fixed->getAttributes().gravityAffected = false;
    PlayerManager::Add("Fixed", fixed);

    auto path = std::make_shared<Player>();
    path->shouldDraw(true);
    path->setMode(Player::Mode::PATH);
    path->getAttributes().gravityAffected = false;
    PlayerManager::Add("Path", path);

    PlayerManager::SetCurrent("Free");
}