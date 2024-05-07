#include <cstdio>
#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <memory>
#include <print>

#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include <glm/ext/quaternion_geometric.hpp>

#include "Config.h"

#include "App.h"
#include "imgui/imgui.h"
#include "utils/Camera.h"
#include "utils/PlayerManager.h"

#include "graphics/Texture.h"
#include "graphics/Color.h"
#include "physics/Collisions.h"
#include "renderables/objects/Player.h"
#include "renderables/objects/Skybox.h"
#include "utils/ShaderManager.h"
#include "graphics/buffers/ShadowBuffer.h"
#include "renderables/objects/ProceduralTerrain.h"
#include "renderables/Particle.h"
#include "renderables/objects/BumperCar.h"
#include "utils/Random.h"
#include "renderables/objects/Trees.h"
#include "graphics/buffers/UniformBuffer.h"
#include "renderables/objects/FerrisWheel.h"
#include "renderables/objects/RollerCoaster.h"
#include "renderables/objects/Walls.h"

// light projection parameters
float near_plane = 1.0F;
float far_plane = 200.0F;
float side_size = 580.0F;

void processInput();

void setupApp();

void setupShaders();

void setupPlayers();

auto main() -> int {
    setupApp();
    setupShaders();
    Texture::Loader::setFlip(true);
    setupPlayers();

    ShaderManager &shaderManager = ShaderManager::GetInstance();
    PlayerManager &playerManager = PlayerManager::GetInstance();

    Skybox skybox;
    const ProceduralTerrain terrain;
    FerrisWheel ferrisWheel;
    const RollerCoaster rollerCoaster;
    const Walls walls;

    const auto pathedCar = playerManager.get("Path")->getCar();
    const auto driveable = playerManager.get("Drive")->getCar();
    const auto duel = playerManager.get("Interactable")->getCar();

    const std::vector models = {
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        pathedCar,
        driveable,
        duel
    };

    for (const auto &model: models) {
        model->addTrackableEntity(Random::Element(models));

        if (Random::Int(0, 5) == 0) {
            model->addTrackableEntity(Random::Element(models));
        }

        if (Random::Int(0, 5) == 0) {
            model->addTrackableEntity(pathedCar);
        }
    }

    float angle = 0.0F;
    const float angleIncrement = glm::radians(360.0F / static_cast<float>(models.size()));

    auto shader = shaderManager.get("Base");
    for (const auto &i: models) {
        constexpr float radius = 30.0F;
        i->setShader(shader);
        const float xPos = 0.0F + radius * glm::cos(angle);
        const float zPos = 0.0F + radius * glm::sin(angle);
        constexpr float yPos = 10.0F;
        const auto translation = glm::vec3(xPos, yPos, zPos);
        glm::mat4 model = translate(Config::IDENTITY_MATRIX, translation);
        model = scale(model, glm::vec3(4.0F));
        i->transform(model);

        angle += angleIncrement;
    }


    shader = shaderManager.get("Untextured");
    for (auto &[name, player]: playerManager.getAll()) {
        player->setShader(shader);
    }

    for (const auto &model: models) {
        model->setPersonShader(shader);
    }

    ferrisWheel.setShader(shader);

    ParticleSystem &particleSystem = ParticleSystem::GetInstance();
    ShadowBuffer shadowBuffer(10000, 10000);
    // ShadowBuffer shadowBuffer(1000, 1000);

    std::vector<glm::vec3> pathPoints;

    for (const auto &model: models) {
        pathPoints.insert(pathPoints.end(), model->getPoints().begin(), model->getPoints().end());
    }

    App::view.setPipeline([&] {
        View::clearTarget(Color::BLACK);
        const auto player = playerManager.getCurrent();
        const auto projectionMatrix = player->getCamera().
                getProjectionMatrix();
        const auto viewMatrix = player->getCamera().getViewMatrix();

        // shadow pass
        shadowBuffer.bind();

        const auto lightPos = skybox.getSun().getPosition();

        // const auto lightView = lookAt(lightPos, player->attributes.position, glm::vec3(0.0F, 1.0F, 0.0F));
        const auto lightProjection = glm::ortho(-side_size, side_size, -side_size, side_size, near_plane, far_plane);
        const auto lightView = lookAt(lightPos, glm::vec3(0.0F), glm::vec3(0.0F, 1.0F, 0.0F));

        shader = shaderManager.get("Shadow");
        shader->use();
        shader->setUniform("view", lightView);
        shader->setUniform("projection", lightProjection);

        for (const auto &model: models) {
            if (model->hasBroke()) {
                continue;
            }
            model->draw(shader);
        }

        playerManager.draw(shader);


        terrain.getTrees().draw(lightView, lightProjection);

        terrain.draw(shader);
        ferrisWheel.draw(shader);
        rollerCoaster.draw(shader);

        shadowBuffer.unbind();

        // render pass
        View::clearTarget(Color::BLACK);

        const auto texture = shadowBuffer.getTexture();
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture);

        shader = shaderManager.get("Base");
        shader->use();
        shader->setUniform(
            "sun.position", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());
        shader->setUniform(
            "lightSpaceMatrix", lightProjection * lightView);
        shader->setUniform("shadowMap", 10);


        for (const auto &model: models) {
            model->draw(viewMatrix, projectionMatrix);
        }

        particleSystem.draw(viewMatrix, projectionMatrix);
        rollerCoaster.draw(viewMatrix, projectionMatrix);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        shader = shaderManager.get("Untextured");
        shader->use();
        shader->setUniform(
            "sun.position", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());
        shader->setUniform(
            "lightSpaceMatrix", lightProjection * lightView);
        shader->setUniform("shadowMap", 0);

        playerManager.draw(viewMatrix, projectionMatrix);
        ferrisWheel.draw(viewMatrix, projectionMatrix);

        shader = shaderManager.get("Tree");
        shader->use();
        shader->setUniform(
            "sun.position", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());
        shader->setUniform(
            "lightSpaceMatrix", lightProjection * lightView);
        shader->setUniform("shadowMap", 0);

        shader = terrain.getShader();
        shader->use();
        shader->setUniform(
            "sun.position", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());
        shader->setUniform(
            "lightSpaceMatrix", lightProjection * lightView);
        shader->setUniform("shadowMap", 0);

        shader->setVec3Array("pathedPoints", pathPoints);
        shader->setUniform(
            "pathedPointsCount", static_cast<int>(pathPoints.size()));
        shader->setUniform(
            "pathDarkness", models[0]->getLaps() / 1000.0F);

        terrain.draw(viewMatrix, projectionMatrix);
        walls.draw(viewMatrix, projectionMatrix);

        if (App::view.highQuality) {
            shader = shaderManager.get("Grass");
            shader->use();
            shader->setUniform(
                "sun.position", skybox.getSun().getPosition());
            shader->setUniform(
                "viewPos", player->getCamera().getPosition());
            shader->setUniform(
                "lightSpaceMatrix", lightProjection * lightView);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            shader->setUniform("shadowMap", 0);
            shader->setUniform("view", viewMatrix);
            shader->setUniform("projection", projectionMatrix);
            if (!App::paused) {
                shader->setUniform("time", App::view.getTime());
            }

            terrain.draw(shader);
        }

        skybox.draw(viewMatrix, projectionMatrix);

        shader = shaderManager.get("PostProcess");
        shader->use();
        shader->setUniform("sunPosition", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());

        // App::view.getPostProcessor().setTexture(texture);

        // post processing
    });

    App::view.setInterface([&] {
        if (App::paused) {
            playerManager.getCurrent()->getCamera().interface();
            playerManager.interface();
            playerManager.getCurrent()->interface();
            App::view.optionsInterface();
            BumperCar::Interface();

            App::debugInterface();
            skybox.getSun().interface();
            particleSystem.interface();

            ImGui::Begin("Shadow Buffer");
            ImGui::SliderFloat("Near Plane", &near_plane, 0.0F, 10.0F);
            ImGui::SliderFloat("Far Plane", &far_plane, 0.0F, 10000.0F);
            ImGui::SliderFloat("Side Size", &side_size, 0.0F, 10000.0F);
            ImGui::End();
        }

        if (App::debug) {
            playerManager.getCurrent()->debug();
            ShaderManager::GetInstance().interface();
            ImGui::Begin("Shadow Buffer");
            ImGui::Image(reinterpret_cast<void *>(shadowBuffer.getTexture()), ImVec2(200, 200));
            ImGui::End();
        }
    });

    try {
        App::loop([&] {
            playerManager.update(App::view.getDeltaTime());
            const auto player = playerManager.getCurrent();

            for (const auto &model: models) {
                model->clearTrackablePositions();
                // add trackable positions of all other models
                for (const auto &m: models) {
                    if (m != model) {
                        model->addTrackablePosition(m->attributes.position);
                    }
                }

                if (player->getMode() != Player::Mode::ORBIT && player->getMode() != Player::Mode::FIXED &&
                    player->getMode() != Player::Mode::PATH && player->getMode() != Player::Mode::FREE) {
                    model->addTrackablePosition(player->attributes.position);
                }
                model->addTrackablePosition(player->attributes.position);

                model->update(App::view.getDeltaTime());
            }

            skybox.update(App::view.getDeltaTime());
            particleSystem.update(App::view.getDeltaTime());
            ferrisWheel.update(App::view.getDeltaTime());

            for (const auto &model: models) {
                if (model == player->getCar() && player->getMode() != Player::Mode::DUEL) {
                    continue;
                }

                if (Physics::Collisions::check(*player, *model)) {
                    if (model == player->getCar()) {
                        player->startDriving(true);
                    }

                    if (length(player->getAttributes().force) > 200.0F && (player->getMode() == Player::Mode::DRIVE ||
                                                                           player->getMode() == Player::Mode::PATH ||
                                                                           player->getMode() == Player::Mode::FPS)) {
                        App::view.blurScreen();
                        model->takeDamage(0.3F);
                    }

                    player->attributes.isColliding = true;
                    model->attributes.isColliding = true;

                    const auto collisionPoint = Physics::Collisions::getCollisionPoint(*player, *model);
                    Physics::Collisions::resolve(*player, *model, collisionPoint);

                    player->collisionResponse();
                    model->collisionResponse();

                    particleSystem.generate(player->attributes.position - collisionPoint,
                                            player->attributes.velocity, Color::SILVER);
                } else {
                    player->attributes.isColliding = false;
                    model->attributes.isColliding = false;
                }
            }

            for (int i = 0; i < models.size(); i++) {
                for (int j = i + 1; j < models.size(); j++) {
                    if (Physics::Collisions::check(*models[i], *models[j])) {
                        models[i]->attributes.isColliding = true;
                        models[j]->attributes.isColliding = true;

                        const auto collisionPoint = Physics::Collisions::getCollisionPoint(*models[i], *models[j]);
                        Physics::Collisions::resolve(*models[i], *models[j], collisionPoint);

                        particleSystem.generate(models[i]->attributes.position - collisionPoint,
                                                models[i]->attributes.velocity, Color::SILVER);

                        models[i]->collisionResponse();
                        models[j]->collisionResponse();

                        if ((models[i] == player->getCar() || models[j] == player->getCar())) {
                            App::view.blurScreen();
                            if (glm::length(player->getAttributes().force) > 200.0F) {
                                models[i]->takeDamage(0.3F);
                                models[j]->takeDamage(0.3F);
                            }
                        }
                    } else {
                        models[i]->attributes.isColliding = false;
                        models[j]->attributes.isColliding = false;
                    }
                }
            }


            for (const auto &[name, player]: playerManager.getAll()) {
                if (Physics::Collisions::check(*player, terrain)) {
                    Physics::Collisions::resolve(*player, terrain);
                    player->attributes.isGrounded = true;
                } else {
                    player->attributes.isGrounded = false;
                }
            }

            for (const auto &model: models) {
                if (Physics::Collisions::check(*model, terrain)) {
                    Physics::Collisions::resolve(*model, terrain);
                    model->attributes.isGrounded = true;
                } else {
                    model->attributes.isGrounded = false;
                }
            }

            for (const Walls::Wall &wall: walls.getWalls()) {
                if (Physics::Collisions::check(player->getBoundingBox(), wall.box)) {
                    Physics::Collisions::resolve(*player, wall.normal);
                }
            }
        });
    } catch (const std::exception &e) {
        std::println(stderr, "{}", e.what());
    }

    App::quit();
}

void processInput() {
    bool moved = false;
    PlayerManager &playerManager = PlayerManager::GetInstance();
    const std::shared_ptr<Player> player = playerManager.getCurrent();

    if (App::view.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS || App::view.getKey(GLFW_KEY_Q) == GLFW_PRESS) {
        App::view.close();
    }

    if (App::view.getKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        App::setPaused(!App::paused);
    }

    if (App::paused &&
        player->getCamera().getMode() != Camera::Mode::ORBIT) {
        return;
    }

    if (App::view.getKey(GLFW_KEY_W) == GLFW_PRESS) {
        moved = true;
        player->processKeyboard(Player::Direction::FORWARD,
                                App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_S) == GLFW_PRESS) {
        moved = true;
        player->processKeyboard(Player::Direction::BACKWARD,
                                App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_A) == GLFW_PRESS) {
        moved = true;
        player->processKeyboard(Player::Direction::LEFT,
                                App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_D) == GLFW_PRESS) {
        moved = true;
        player->processKeyboard(Player::Direction::RIGHT,
                                App::view.getDeltaTime());
    }

    if (!moved) {
        player->processKeyboard(Player::Direction::NONE,
                                App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        player->processKeyboard(Player::Direction::DOWN,
                                App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_SPACE) == GLFW_PRESS) {
        player->processKeyboard(Player::Direction::UP,
                                App::view.getDeltaTime());
    }

    if (App::view.getKey(GLFW_KEY_N) == GLFW_PRESS) {
        player->nitro();
    }
}

void setupApp() {
    App::window("Coursework", Config::DEFAULT_WIDTH, Config::DEFAULT_HEIGHT);
    App::init();

    PlayerManager &playerManager = PlayerManager::GetInstance();

    App::view.setKey(processInput);
    App::view.setMouse([&] {
        if (!App::paused) {
            playerManager.getCurrent()->getCamera().processMouseMovement(
                App::view.getMouseOffsetX(), App::view.getMouseOffsetY());
        }
    });

    App::view.setScroll([&] {
        playerManager.getCurrent()->getCamera().processMouseScroll(
            App::view.getScrollY());
    });

    App::view.setResize([&] {
        playerManager.setAspect(static_cast<float>(App::view.getWidth()) /
                                static_cast<float>(App::view.getHeight()));
        glViewport(0, 0, static_cast<GLsizei>(App::view.getWidth()),
                   static_cast<GLsizei>(App::view.getHeight()));
    });
}

void setupShaders() {
    ShaderManager &shaderManager = ShaderManager::GetInstance();

    shaderManager.add("Simple", "../Assets/shaders/terrain.vert", "../Assets/shaders/terrain.frag");
    shaderManager.add("Base", "../Assets/shaders/textured.vert", "../Assets/shaders/textured.frag",
                      "../Assets/shaders/textured.geom");
    shaderManager.add("PostProcess", "../Assets/shaders/postProcessing.vert", "../assets/shaders/postProcessing.frag");
    shaderManager.add("Sky", "../Assets/shaders/Sky.vert", "../Assets/shaders/Sky.frag");
    shaderManager.add("Sun", "../Assets/shaders/sun.vert", "../Assets/shaders/sun.frag");
    shaderManager.add("BoundingBox", "../Assets/shaders/boundingBox.vert", "../Assets/shaders/boundingBox.frag");
    shaderManager.add("Shadow", "../Assets/shaders/shadow.vert", "../Assets/shaders/shadow.frag");
    shaderManager.add("Particle", "../Assets/shaders/particle.vert", "../Assets/shaders/particle.frag");
    shaderManager.add("Grass", "../Assets/shaders/grass.vert", "../Assets/shaders/grass.frag",
                      "../Assets/shaders/grass.geom");
    shaderManager.add("Tree", "../Assets/shaders/tree.vert", "../Assets/shaders/tree.frag");
    shaderManager.add("Untextured", "../Assets/shaders/materialed.vert", "../Assets/shaders/materialed.frag");

    auto shader = shaderManager.get("PostProcess");
    shader->use();
    shader->setUniform("screenTexture", 0);

    shader = shaderManager.get("Base");
    shader->use();

    shader->setUniform("sun.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
    shader->setUniform("sun.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
    shader->setUniform("sun.specular", glm::vec3(1.0F, 1.0F, 1.0F));

    shader = shaderManager.get("Untextured");
    shader->use();

    shader->setUniform("sun.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
    shader->setUniform("sun.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
    shader->setUniform("sun.specular", glm::vec3(1.0F, 1.0F, 1.0F));

    shader = shaderManager.get("Tree");
    shader->use();

    shader->setUniform("sun.ambient", glm::vec3(0.8F, 0.8F, 0.8F));
    shader->setUniform("sun.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
    shader->setUniform("sun.specular", glm::vec3(1.0F, 1.0F, 1.0F));
}

void setupPlayers() {
    PlayerManager &playerManager = PlayerManager::GetInstance();

    const auto orbit = std::make_shared<Player>(Player::Mode::ORBIT);
    orbit->shouldDraw(false);
    orbit->getCamera().setOrbit(glm::vec3(0.0F, 0.0F, 0.0F), 50.0F, 0.0F, 3.0F, 10.0F);
    playerManager.add("Orbit", orbit);

    const auto free = std::make_shared<Player>(Player::Mode::FREE);
    free->shouldDraw(false);
    playerManager.add("Free", free);

    const auto fps = std::make_shared<Player>(Player::Mode::FPS);
    fps->shouldDraw(true);
    playerManager.add("FPS", fps);

    const auto fixed = std::make_shared<Player>(Player::Mode::FIXED);
    fixed->shouldDraw(false);
    fixed->getCamera().setFixed(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 10.0F, 10.0F));
    playerManager.add("Fixed", fixed);

    const auto path = std::make_shared<Player>(Player::Mode::PATH);
    path->shouldDraw(false);
    path->getCar()->setMode(BumperCar::Mode::AUTO);
    playerManager.add("Path", path);

    const auto drive = std::make_shared<Player>(Player::Mode::DRIVE);
    playerManager.add("Drive", drive);

    const auto interactable = std::make_shared<Player>(Player::Mode::DUEL);
    playerManager.add("Interactable", interactable);


    playerManager.setCurrent("Free");
}
