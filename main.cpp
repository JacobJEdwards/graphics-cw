#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <iostream>
#include <memory>

#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include <glm/ext/quaternion_geometric.hpp>

#include "Config.h"

#include "App.h"
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

void processInput();

void setupApp();

void setupShaders();

void setupPlayers();

auto main() -> int {
    setupApp();
    setupShaders();
    setupPlayers();

    Skybox skybox;
    const ProceduralTerrain terrain;

    Texture::Loader::setFlip(true);

    const auto pathedCar = PlayerManager::Get("Path")->getCar();
    pathedCar->shouldDrawPlayer(false);

    const auto driveable = PlayerManager::Get("Drive")->getCar();

    const std::vector models = {
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        pathedCar,
        driveable
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

    auto matrix = Config::IDENTITY_MATRIX;
    constexpr auto newPosition = glm::vec3(20.0F, 10.0F, 8.0F);
    matrix = translate(matrix, newPosition);
    matrix = scale(matrix, glm::vec3(4.0F));

    auto shader = ShaderManager::Get("Base");
    for (const auto &model: models) {
        model->setShader(shader);
        model->transform(matrix);
        matrix = translate(matrix, glm::vec3(5.0F, 0.0F, 0.0F));
    }

    PlayerManager::Get("Orbit")->setShader(shader);
    PlayerManager::Get("Free")->setShader(shader);
    PlayerManager::Get("FPS")->setShader(shader);
    PlayerManager::Get("Fixed")->setShader(shader);
    PlayerManager::Get("Path")->setShader(shader);
    PlayerManager::Get("Drive")->setShader(shader);

    ParticleSystem &particleSystem = ParticleSystem::GetInstance();
    ShadowBuffer shadowBuffer(App::view.getWidth(), App::view.getHeight());
    // ShadowBuffer shadowBuffer(10000, 10000);

    std::vector<glm::vec3> points;

    for (const auto &model: models) {
        points.insert(points.end(), model->getPoints().begin(), model->getPoints().end());
    }

    std::cout << "Points: " << points.size() << std::endl;

    App::view.setPipeline([&] {
        View::clearTarget(Color::BLACK);
        const auto player = PlayerManager::GetCurrent();
        const auto projectionMatrix = player->getCamera().getProjectionMatrix();
        const auto viewMatrix = player->getCamera().getViewMatrix();

        // shadow pass
        shadowBuffer.bind();

        const auto lightPos = skybox.getSun().getPosition();
        const auto lightProjection = glm::ortho(-100.0F, 100.0F, -100.0F, 100.0F, 1.0F, 200.0F);
        const auto lightView = lookAt(lightPos, player->getPosition(), glm::vec3(0.0F, 1.0F, 0.0F));

        shader = ShaderManager::Get("Shadow");
        shader->use();
        shader->setUniform("view", lightView);
        shader->setUniform("projection", lightProjection);

        for (const auto &model: models) {
            model->draw(shader);
        }


        PlayerManager::Draw(shader);

        terrain.draw(shader);

        shadowBuffer.unbind();

        // render pass
        View::clearTarget(Color::BLACK);

        shader = ShaderManager::Get("Base");
        shader->use();
        shader->setUniform("light.position", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());

        const auto texture = shadowBuffer.getTexture();

        for (const auto &model: models) {
            model->draw(viewMatrix, projectionMatrix);
        }

        // driveable->draw(viewMatrix, projectionMatrix);

        particleSystem.draw(viewMatrix, projectionMatrix);

        PlayerManager::Draw(viewMatrix, projectionMatrix);
        particleSystem.draw(viewMatrix, projectionMatrix);

        shader = terrain.getShader();
        shader->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader->setUniform("light.position", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());
        shader->setUniform("lightSpaceMatrix", lightProjection * lightView);
        shader->setUniform("shadowMap", 0);

        // give points to terrain
        shader->setVec3Array("pathedPoints", points);
        shader->setUniform("pathedPointsCount", static_cast<int>(points.size()));
        shader->setUniform("pathDarkness", models[0]->getLaps() / 1000.0F);

        terrain.draw(viewMatrix, projectionMatrix);
        skybox.draw(viewMatrix, projectionMatrix);

        shader = ShaderManager::Get("PostProcess");
        shader->use();
        shader->setUniform("sunPosition", skybox.getSun().getPosition());
        shader->setUniform("viewPos", player->getCamera().getPosition());
    });

    App::view.setInterface([&] {
        if (App::paused) {
            PlayerManager::GetCurrent()->getCamera().interface();
            PlayerManager::Interface();
            PlayerManager::GetCurrent()->interface();
            App::view.optionsInterface();
            BumperCar::Interface();

            App::debugInterface();
            skybox.getSun().interface();
            particleSystem.interface();
        }

        if (App::debug) {
            PlayerManager::GetCurrent()->debug();
            ShaderManager::Interface();
        }
    });

    try {
        App::loop([&] {
            PlayerManager::Update(App::view.getDeltaTime());
            const auto player = PlayerManager::GetCurrent();

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

            for (const auto &model: models) {
                if (model == player->getCar()) {
                    continue;
                }

                if (Physics::Collisions::check(*player, *model)) {
                    if (length(player->getAttributes().force) > 25.0F && (player->getMode() == Player::Mode::DRIVE ||
                                                                          player->getMode() == Player::Mode::PATH ||
                                                                          player->getMode() == Player::Mode::FPS)) {
                        App::view.blurScreen();
                        model->setBroken(true);
                    }

                    player->attributes.isColliding = true;
                    model->attributes.isColliding = true;

                    const auto collisionPoint = Physics::Collisions::getCollisionPoint(*player, *model);
                    Physics::Collisions::resolve(*player, *model, collisionPoint);

                    particleSystem.generate(player->attributes.position - collisionPoint,
                                            player->attributes.velocity, Color::ORANGE);
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

                        particleSystem.generate(models[i]->attributes.position + collisionPoint,
                                                models[i]->attributes.velocity, Color::ORANGE);

                        /*
                        if (length(models[i]->attributes.force) > 25.0F) {
                            models[j]->setBroken(true);
                        }

                        if (length(models[j]->attributes.force) > 25.0F) {
                            models[i]->setBroken(true);
                        }
                        */

                        // if player car, blur screen
                        if ((models[i] == player->getCar() || models[j] == player->getCar())) {
                            models[i]->setBroken(true);
                            models[j]->setBroken(true);
                            App::view.blurScreen();
                        }
                    } else {
                        models[i]->attributes.isColliding = false;
                        models[j]->attributes.isColliding = false;
                    }
                }
            }


            for (const auto &player: PlayerManager::GetAll()) {
                if (Physics::Collisions::check(*player.second, terrain)) {
                    Physics::Collisions::resolve(*player.second, terrain);
                    player.second->attributes.isGrounded = true;
                } else {
                    player.second->attributes.isGrounded = false;
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
    const auto orbit = std::make_shared<Player>(Player::Mode::ORBIT);
    orbit->shouldDraw(false);
    orbit->getCamera().setOrbit(glm::vec3(0.0F, 0.0F, 0.0F), 50.0F, 0.0F, 3.0F, 10.0F);
    orbit->attributes.gravityAffected = false;
    PlayerManager::Add("Orbit", orbit);

    const auto free = std::make_shared<Player>(Player::Mode::FREE);
    free->shouldDraw(true);
    free->getAttributes().gravityAffected = false;
    PlayerManager::Add("Free", free);

    const auto fps = std::make_shared<Player>(Player::Mode::FPS);
    fps->shouldDraw(true);
    PlayerManager::Add("FPS", fps);

    const auto fixed = std::make_shared<Player>(Player::Mode::FIXED);
    fixed->shouldDraw(true);
    fixed->getCamera().setFixed(glm::vec3(0.0F, 0.0F, 0.0F), glm::vec3(0.0F, 10.0F, 10.0F));
    fixed->attributes.gravityAffected = false;
    PlayerManager::Add("Fixed", fixed);

    const auto path = std::make_shared<Player>(Player::Mode::PATH);
    path->shouldDraw(true);
    path->attributes.gravityAffected = true;
    PlayerManager::Add("Path", path);

    const auto drive = std::make_shared<Player>(Player::Mode::DRIVE);
    drive->shouldDraw(true);
    drive->attributes.gravityAffected = true;
    PlayerManager::Add("Drive", drive);

    PlayerManager::SetCurrent("Free");
}
