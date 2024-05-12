#include <algorithm>
#include <cstdio>
#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>
#include <print>

#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include <glm/ext/quaternion_geometric.hpp>
#include <graphics/buffers/DepthBuffer.h>

#include "Config.h"

#include "App.h"
#include "imgui/imgui.h"
#include "renderables/Entity.h"
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
#include "renderables/objects/Scene.h"
#include "renderables/objects/Walls.h"

// light projection parameters
float near_plane = 1.0F;
float far_plane = 200.0F;
float side_size = 580.0F;

void processInput();

void setupApp();

void setupShaders();

void setupPlayers();

struct Matrices {
    glm::mat4 view = Config::IDENTITY_MATRIX;
    glm::mat4 projection = Config::IDENTITY_MATRIX;
    glm::mat4 lightSpaceMatrix = Config::IDENTITY_MATRIX;
};

struct DirectionalLight {
    glm::vec3 direction = glm::vec3(-0.2F, -1.0F, -0.3F);
    glm::vec3 ambient = glm::vec3(0.05F, 0.05F, 0.05F);
    glm::vec3 diffuse = glm::vec3(0.4F, 0.4F, 0.4F);
    glm::vec3 specular = glm::vec3(0.5F, 0.5F, 0.5F);
};

struct PointLight {
    glm::vec3 position = glm::vec3(0.0F, 0.0F, 0.0F);
    glm::vec3 ambient = glm::vec3(0.05F, 0.05F, 0.05F);
    glm::vec3 diffuse = glm::vec3(0.8F, 0.8F, 0.8F);
    glm::vec3 specular = glm::vec3(1.0F, 1.0F, 1.0F);

    float constant = 1.0F;
    float linear = 0.09F;
    float quadratic = 0.032F;
};

struct SpotLight {
    glm::vec3 position = glm::vec3(0.0F, 0.0F, 0.0F);
    glm::vec3 direction = glm::vec3(0.0F, 0.0F, 0.0F);
    glm::vec3 ambient = glm::vec3(0.0F, 0.0F, 0.0F);
    glm::vec3 diffuse = glm::vec3(1.0F, 1.0F, 1.0F);
    glm::vec3 specular = glm::vec3(1.0F, 1.0F, 1.0F);

    float constant = 1.0F;
    float linear = 0.09F;
    float quadratic = 0.032F;

    float cutOff = glm::cos(glm::radians(12.5F));
    float outerCutOff = glm::cos(glm::radians(15.0F));
};

struct Lights {
    DirectionalLight sun = DirectionalLight();
    /*
    PointLight pointLight[4] = {
        PointLight(),
        PointLight(),
        PointLight(),
        PointLight()
    };
    int pointLightCount = 0;
    SpotLight spotLight = SpotLight();
    */
};

struct CameraInfo {
    glm::vec3 position = glm::vec3(0.0F, 0.0F, 0.0F);
    glm::vec3 direction = glm::vec3(0.0F, 0.0F, 0.0F);
};

auto main() -> int {
    setupApp();
    setupShaders();
    Texture::Loader::setFlip(true);
    setupPlayers();

    ShaderManager &shaderManager = ShaderManager::GetInstance();
    PlayerManager &playerManager = PlayerManager::GetInstance();

    const Walls walls;
    Scene scene;

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
        std::make_shared<BumperCar>(),
        std::make_shared<BumperCar>(),
        pathedCar,
        driveable,
        duel
    };

    for (const auto &model: models) {
        model->addTrackableEntity(Random::Element(models));
        model->addTrackableEntity(PlayerManager::GetInstance().get("FPS"));

        if (Random::Int(0, 5) == 0) {
            model->addTrackableEntity(Random::Element(models));
        }

        if (Random::Int(0, 10) == 0) {
            model->addTrackableEntity(Random::Element(models));
        }

        if (Random::Int(0, 10) == 0) {
            model->addTrackableEntity(pathedCar);
        }

        if (Random::Int(0, 25) == 0) {
            model->setMode(BumperCar::Mode::RANDOM);
        }

        if (Random::Int(0, 100) == 0) {
            model->setMode(BumperCar::Mode::FOLLOW);
        }

        if (Random::Int(0, 50) == 0) {
            model->setMode(BumperCar::Mode::NONE);
        }
    }

    float angle = 0.0F;
    const float angleIncrement = glm::radians(360.0F / static_cast<float>(models.size()));

    for (const auto &i: models) {
        constexpr float radius = 30.0F;
        const float xPos = 0.0F + radius * glm::cos(angle);
        const float zPos = 0.0F + radius * glm::sin(angle);
        constexpr float yPos = 10.0F;
        const auto translation = glm::vec3(xPos, yPos, zPos);
        glm::mat4 model = translate(Config::IDENTITY_MATRIX, translation);
        model = scale(model, glm::vec3(4.0F));
        i->transform(model);

        angle += angleIncrement;
    }

    ParticleSystem &particleSystem = ParticleSystem::GetInstance();
    ShadowBuffer shadowBuffer(10000, 10000);
    DepthBuffer viewBuffer(App::view.getWidth(), App::view.getHeight());
    // DepthBuffer viewBuffer(10000, 10000);

    std::vector<glm::vec3> pathPoints;

    for (const auto &model: models) {
        pathPoints.insert(pathPoints.end(), model->getPoints().begin(), model->getPoints().end());
    }

    auto shader = scene.getTerrain()->getShader();
    for (std::size_t i = 0; i < pathPoints.size(); i++) {
        shader->setUniform("pathedPoints[" + std::to_string(i) + "]", pathPoints[i]);
    }

    shader->setUniform("pathedPointsCount", static_cast<int>(pathPoints.size()));


    std::vector<std::shared_ptr<Entity> > entities;
    entities.insert(entities.end(), models.begin(), models.end());

    for (const auto &[name, player]: playerManager.getAll()) {
        entities.push_back(player);
    }

    entities.push_back(scene.getFerrisWheel());


    // matrices ubo setup
    GLuint matricesUBO;
    glGenBuffers(1, &matricesUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrices), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, matricesUBO);

    Matrices matrices;
    matrices.view = Config::IDENTITY_MATRIX;
    matrices.projection = Config::IDENTITY_MATRIX;
    matrices.lightSpaceMatrix = Config::IDENTITY_MATRIX;

    // lights ubo setup
    GLuint lightsUBO;
    glGenBuffers(1, &lightsUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Lights), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, lightsUBO);

    Lights lights;
    lights.sun.direction = glm::vec3(-0.2F, -1.0F, -0.3F);
    lights.sun.ambient = glm::vec3(1.0F);
    lights.sun.diffuse = glm::vec3(1.0F);
    lights.sun.specular = glm::vec3(1.0F);

    GLuint cameraUBO;
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraInfo), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, cameraUBO);

    CameraInfo cameraInfo;

    // set the block binding points
    for (const auto &[name, shader]: shaderManager.getAll()) {
        shader->use();
        const auto matricesIndex = glGetUniformBlockIndex(shader->getProgramID(), "Matrices");
        glUniformBlockBinding(shader->getProgramID(), matricesIndex, 0);

        const auto lightsIndex = glGetUniformBlockIndex(shader->getProgramID(), "Lights");
        glUniformBlockBinding(shader->getProgramID(), lightsIndex, 1);

        const auto cameraIndex = glGetUniformBlockIndex(shader->getProgramID(), "Camera");
        glUniformBlockBinding(shader->getProgramID(), cameraIndex, 2);
    }

    App::view.setPipeline([&] {
        View::clearTarget(Color::BLACK);
        const auto player = playerManager.getCurrent();
        const auto projectionMatrix = player->getCamera().
                getProjectionMatrix();
        const auto projectionMatrixDepth = player->getCamera().
                getProjectionDepthMatrix();

        const auto viewMatrix = player->getCamera().getViewMatrix();
        const auto sunPos = scene.getSkybox()->getSun().getPosition();

        const auto viewPos = player->getCamera().getPosition();
        const auto viewDir = player->getCamera().getFront();

        cameraInfo.position = viewPos;
        cameraInfo.direction = viewDir;

        glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraInfo), &cameraInfo);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // shadow pass
        shadowBuffer.bind();

        const auto lightProjection = glm::ortho(-side_size, side_size, -side_size, side_size, near_plane, far_plane);
        const auto lightView = lookAt(sunPos / 20.0F, glm::vec3(0.0F), glm::vec3(0.0F, 1.0F, 0.0F));
        const auto lightSpaceMatrix = lightProjection * lightView;

        matrices.view = lightView;
        matrices.projection = lightProjection;
        matrices.lightSpaceMatrix = lightSpaceMatrix;

        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrices), &matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // lights ubo
        lights.sun.direction = scene.getSkybox()->getSun().getDirection();
        lights.sun.ambient = scene.getSkybox()->getSun().getAmbient();
        lights.sun.diffuse = scene.getSkybox()->getSun().getDiffuse();
        lights.sun.specular = scene.getSkybox()->getSun().getSpecular();

        glBindBuffer(GL_UNIFORM_BUFFER, lightsUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Lights), &lights);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);


        shader = shaderManager.get("Shadow");
        shader->use();

        for (const auto &model: models) {
            if (model->hasBroke()) {
                continue;
            }
            model->draw(shader);
        }

        for (const auto &[name, player]: playerManager.getAll()) {
            player->draw(shader);
        }

        scene.getTerrain()->getTrees().draw();
        scene.getTerrain()->getClouds().draw();
        scene.draw(shader);

        shadowBuffer.unbind();

        viewBuffer.bind();

        DepthBuffer::Clear();

        matrices.view = viewMatrix;
        matrices.projection = projectionMatrixDepth;

        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrices), &matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        shader->use();

        for (const auto &model: models) {
            model->draw(shader);
        }

        playerManager.draw(shader);

        scene.getTerrain()->getTrees().draw();
        scene.getTerrain()->getClouds().draw();
        scene.draw(shader);

        viewBuffer.unbind();

        matrices.view = viewMatrix;
        matrices.projection = projectionMatrix;
        matrices.lightSpaceMatrix = lightSpaceMatrix;

        glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrices), &matrices);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        View::clearTarget(Color::BLACK);

        auto texture = shadowBuffer.getTexture();
        glActiveTexture(GL_TEXTURE10);
        glBindTexture(GL_TEXTURE_2D, texture);

        for (const auto &[name, shader]: shaderManager.getAll()) {
            shader->use();
            shader->setUniform("shadowMap", 10);
            if (!App::paused) {
                shader->setUniform("time", App::view.getTime());
            }
        }

        for (const auto &model: models) {
            model->draw();
        }

        particleSystem.draw();
        playerManager.draw();

        shader = scene.getTerrain()->getShader();
        shader->use();
        shader->setUniform(
            "pathDarkness", models[0]->getLaps() / 1000.0F);
        for (std::size_t i = 0; i < pathPoints.size(); i++) {
            shader->setUniform("pathedPoints[" + std::to_string(i) + "]", pathPoints[i]);
        }
        shader->setUniform("pathedPointsCount", static_cast<int>(pathPoints.size()));

        shader = shaderManager.get("Untextured");
        shader->use();
        shader->setUniform("color", glm::vec3(1.0F, 1.0F, 1.0F));
        scene.draw();
        walls.draw();


        if (App::view.highQuality) {
            shader = shaderManager.get("Grass");
            scene.getTerrain()->draw(shader);
        }


        texture = viewBuffer.getTexture();
        shader = shaderManager.get("PostProcess");
        shader->use();

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);

        shader->setUniform("depthTexture", 1);

        // App::view.getPostProcessor().setTexture(texture);
    });

    App::view.setInterface([&] {
        if (App::paused) {
            playerManager.getCurrent()->getCamera().interface();
            playerManager.interface();
            playerManager.getCurrent()->interface();
            App::view.optionsInterface();
            BumperCar::Interface();

            App::debugInterface();
            scene.getSkybox()->getSun().interface();
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
            ImGui::Begin("View Buffer");
            ImGui::Image(reinterpret_cast<void *>(viewBuffer.getTexture()), ImVec2(200, 200));
            ImGui::End();
        }
    });

    try {
        App::loop([&] {
            const auto player = playerManager.getCurrent();

            for (const auto &model: models) {
                model->clearTrackablePositions();

                if (model == player->getCar() && player->getMode() != Player::Mode::DUEL) {
                    continue;
                }

                if (Physics::Collisions::check(*player, *model)) {
                    if (model == player->getCar()) {
                        player->startDriving(true);
                    }

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

            particleSystem.update(App::view.getDeltaTime());
            scene.update(App::view.getDeltaTime());

            for (int i = 0; i < models.size(); i++) {
                for (int j = i + 1; j < models.size(); j++) {
                    if (Physics::Collisions::check(*models[i], *models[j])) {
                        const auto collisionPoint = Physics::Collisions::getCollisionPoint(*models[i], *models[j]);
                        Physics::Collisions::resolve(*models[i], *models[j], collisionPoint);

                        particleSystem.generate(models[i]->attributes.position - collisionPoint,
                                                models[i]->attributes.velocity, Color::SILVER);

                        models[i]->collisionResponse();
                        models[j]->collisionResponse();
                    } else {
                        models[i]->attributes.isColliding = false;
                        models[j]->attributes.isColliding = false;
                    }
                }
            }

            for (const auto &[name, player]: playerManager.getAll()) {
                if (Physics::Collisions::check(*player, *scene.getTerrain())) {
                    Physics::Collisions::resolve(*player, *scene.getTerrain());
                    player->attributes.isGrounded = true;
                } else {
                    player->attributes.isGrounded = false;
                }
            }

            for (const auto &model: models) {
                if (Physics::Collisions::check(*model, *scene.getTerrain())) {
                    Physics::Collisions::resolve(*model, *scene.getTerrain());
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

            for (const auto &model: models) {
                for (const Walls::Wall &wall: walls.getWalls()) {
                    if (Physics::Collisions::check(model->getBoundingBox(), wall.box)) {
                        Physics::Collisions::resolve(*model, wall.normal);
                    }
                }
            }

            for (const auto &enitity: entities) {
                enitity->update(App::view.getDeltaTime());
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
    shaderManager.add("Cloud", "../Assets/shaders/cloud.vert", "../Assets/shaders/cloud.frag");
    shaderManager.add("Moon", "../Assets/shaders/Moon.vert", "../Assets/shaders/Moon.frag");

    const auto shader = shaderManager.get("PostProcess");
    shader->use();
    shader->setUniform("screenTexture", 0);

    for (const auto &[name, shader]: shaderManager.getAll()) {
        shader->use();
        shader->setUniform("sun.ambient", Color::WHITE);
        shader->setUniform("sun.diffuse", Color::ORANGE);
        shader->setUniform("sun.specular", Color::WHITE);
    }
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

    // TODO fix
    const auto interactable = std::make_shared<Player>(Player::Mode::DUEL);
    playerManager.add("Interactable", interactable);


    playerManager.setCurrent("Free");
}
