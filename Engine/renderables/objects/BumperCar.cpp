//
// Created by Jacob Edwards on 05/04/2024.
//

#include "BumperCar.h"

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <limits>
#include <memory>
#include <vector>
#include "graphics/Color.h"
#include "graphics/Model.h"
#include "physics/Spline.h"
#include "utils/PlayerManager.h"
#include "renderables/Entity.h"
#include "App.h"
#include "graphics/Shader.h"
#include "imgui/imgui.h"
#include "utils/Random.h"
#include <GL/glew.h>
#include <utils/ShaderManager.h>

#include "renderables/Particle.h"

float BumperCar::coneRadius = 45.0F;
float BumperCar::coneHeight = 100.0F;
bool BumperCar::paused = false;
float BumperCar::trackingDistance = 25.0F;
float BumperCar::ventureDistance = 100.0F;

BumperCar::BumperCar(const glm::vec2 centre, const float radius, const float speed) : Entity(
        "../Assets/objects/bumpercar1/bumper-car.obj"), speed(speed),
    person("../Assets/objects/person-sitting/person.obj") {
    const int numPoints = Random::Int(10, 30);

    float angle = 0.0F;
    const float angleIncrement = glm::radians(360.0F / static_cast<float>(numPoints));

    for (int i = 0; i < numPoints; i++) {
        const float xPos = centre.x + radius * glm::cos(angle) * Random::Float(0.9F, 1.1F);
        const float zPos = centre.y + radius * glm::sin(angle) * Random::Float(0.9F, 1.1F);
        points.emplace_back(xPos, 0.0F, zPos);
        angle += angleIncrement;
    }

    Physics::Spline::Type type;

    switch (Random::Int(0, 4)) {
        case 0:
            type = Physics::Spline::Type::CATMULLROM;
            break;
        case 1:
            type = Physics::Spline::Type::BEZIER;
            break;
        case 2:
            type = Physics::Spline::Type::HERMITE;
            break;
        case 3:
            type = Physics::Spline::Type::CUBIC;
            break;
        case 4:
            type = Physics::Spline::Type::LINEAR;
            break;
        default:
            type = Physics::Spline::Type::CATMULLROM;
            break;
    }


    this->speed *= Random::Float(0.8F, 1.2F);

    spline = Physics::Spline(points, type, this->speed);
    spline.randomise();

    attributes.mass = 10.0F;
    generateTexture();
}

BumperCar::BumperCar(std::vector<glm::vec3> points, const float speed) : Entity(
                                                                             "../Assets/objects/bumpercar1/bumper-car.obj"),
                                                                         speed(speed),
                                                                         spline(
                                                                             points, Physics::Spline::Type::CATMULLROM,
                                                                             speed), points(points), person(
                                                                             "../Assets/objects/person-sitting/person.obj") {
    spline.randomise();
    this->speed *= Random::Float(0.8F, 1.2F);
    attributes.mass = 10.0F;
    generateTexture();
}

auto BumperCar::getLaps() const -> std::size_t {
    return spline.getLaps();
}

void BumperCar::addTrackablePosition(const glm::vec3 position) {
    trackablePositions.push_back(position);
}


void BumperCar::setPersonShader(const std::shared_ptr<Shader> &shader) {
    personShader = shader;
}

void BumperCar::reset() {
    spline = Physics::Spline(points, Physics::Spline::Type::CATMULLROM, speed);
    spline.randomise();
    isBroken = false;
    brokenTime = 0.0F;
    isExploding = false;
    explodeTime = 0.0F;

    attributes.position = points[0];
    attributes.velocity = glm::vec3(0.0F);
    attributes.acceleration = glm::vec3(0.0F);
    attributes.rotation = glm::vec3(0.0F);
    attributes.angularVelocity = glm::vec3(0.0F);
    attributes.angularAcceleration = glm::vec3(0.0F);
    attributes.isColliding = false;
    attributes.mass = 10.0F;
    damageTaken = 0.0F;
}

void BumperCar::generateTexture() {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    damageTexture.id = texture;

    std::vector<unsigned char> data(4 * 512 * 512, 255);
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            // random noise
            data[4 * (512 * i + j) + 0] = Random::Int(0, 255);
            data[4 * (512 * i + j) + 1] = Random::Int(0, 255);
            data[4 * (512 * i + j) + 2] = Random::Int(0, 255);
            data[4 * (512 * i + j) + 3] = 255;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    // parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// add random particles
void BumperCar::update(const float deltaTime) {
    if (paused) {
        return;
    }

    if (isExploding) {
        explodeTime += deltaTime;
    }

    if (damageTaken >= 1.0F) {
        isBroken = true;
    }

    if (isBroken) {
        brokenTime += deltaTime;
        if (brokenTime >= 5.0F) {
            isExploding = true;
        }

        if (brokenTime >= 10.0F) {
            reset();
        } else if (!isExploding) {
            ParticleSystem::GetInstance().generate(attributes.position, glm::vec3(1.0F, 10.0F, 1.0F), Color::ORANGE,
                                                   25);
        }
        Entity::update(deltaTime);
        return;
    }

    if (Random::Int(0, 150) == 0) {
        ParticleSystem::GetInstance().generate(attributes.position, -attributes.velocity / 5.0F, Color::WHITE, 10);
    }

    // be slightly on fire if damaged
    if (damageTaken > 0.0F) {
        ParticleSystem::GetInstance().generate(attributes.position, glm::vec3(-attributes.velocity.x, 8.0F,
                                                                              -attributes.velocity.y),
                                               Color::ORANGE,
                                               static_cast<int>(damageTaken * 5.0F));
    }


    const auto point = spline.getPoint();
    const auto currentPos = attributes.position;

    const auto forward = attributes.getFront();

    if (!attributes.isGrounded) {
        Entity::update(deltaTime);
        return;
    }


    switch (mode) {
        case Mode::PATHED:
            spline.update(deltaTime);
            moveTo(point);
            break;
        case Mode::TRACK:
            moveTo(PlayerManager::GetInstance().getCurrent()->getPosition());
            break;
        case Mode::AUTO: {
            spline.update(deltaTime);

            if (attributes.isColliding) {
                moveTo(point);
                break;
            }

            const float adjustedConeRadius = coneRadius * Random::Float(0.8F, 1.2F);

            auto closest = glm::vec3(0.0F);
            auto closestDistance = std::numeric_limits<float>::max();

            std::vector<glm::vec3> positionList(trackablePositions.size() + trackableEntities.size() + 1U);

            for (const auto &entity: trackableEntities) {
                positionList.emplace_back(entity->attributes.getPosition());
            }

            positionList.insert(positionList.end(), trackablePositions.begin(), trackablePositions.end());

            for (const auto &trackablePosition: positionList) {
                auto toPos = normalize(trackablePosition - currentPos);
                if (const auto angle = glm::degrees(glm::acos(dot(forward, toPos))); angle > adjustedConeRadius) {
                    continue;
                }

                if (const auto distance = glm::distance(currentPos, trackablePosition); distance < closestDistance) {
                    closest = trackablePosition;
                    closestDistance = distance;
                }
            }

            if (closestDistance < trackingDistance && distance(currentPos, point) < ventureDistance &&
                closest != glm::vec3(0.0F)) {
                moveTo(closest);
            } else {
                moveTo(point);
            }
            break;
        }
        case Mode::NONE:
            break;
    }

    Entity::update(deltaTime);
}

void BumperCar::takeDamage(const float damage) {
    this->damageTaken += damage;
}

void BumperCar::addTrackableEntity(const std::shared_ptr<Entity> &entity) {
    if (entity == nullptr) {
        return;
    }

    if (std::ranges::find(trackableEntities, entity) != trackableEntities.end()) {
        return;
    }

    if (entity.get() == this) {
        return;
    }

    trackableEntities.push_back(entity);
}

void BumperCar::clearTrackablePositions() {
    trackablePositions.clear();
}


void BumperCar::draw(const std::shared_ptr<Shader> shader) const {
    personShader->use();
    auto mat = attributes.transform;
    // tranlte back based on front of car
    mat = glm::translate(mat, glm::vec3(0.0F, -0.25F, -0.45F));

    mat = glm::scale(mat, glm::vec3(0.35F));
    personShader->setUniform("model", mat);

    if (!isPlayer) {
        personShader->setUniform("color", glm::vec4(1.0F, 1.0F, 1.0F, 1.0F));
    } else {
        personShader->setUniform("color", glm::vec4(1.0F, 0.0F, 0.0F, 1.0F));
    }

    if (drawPlayer && !isBroken) {
        person.draw(personShader);
    }

    shader->use();
    // damage texture
    mat = attributes.transform;
    mat = glm::rotate(mat, glm::radians(-90.0F), glm::vec3(0.0F, 1.0F, 0.0F));
    shader->setUniform("model", mat);
    shader->setUniform("time", explodeTime / 2.0F);
    shader->setUniform("damage", damageTaken);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, damageTexture.id);
    shader->setUniform("damageTexture", 1);

    model->draw(shader);

    if (App::debug) {
        spline.draw(shader);
    }
}

auto BumperCar::getPoints() const -> const std::vector<glm::vec3> & {
    return points;
}


void BumperCar::moveTo(const glm::vec3 position) {
    auto forceNeeded =
            attributes.calculateForce(position);

    forceNeeded = glm::vec3(forceNeeded.x, 0.0F, forceNeeded.z);

    glm::vec3 rotation = attributes.calculateRotation(position);
    rotation = glm::vec3(0.0F, rotation.y, 0.0F);

    attributes.applyRotation(rotation);
    attributes.applyForce(forceNeeded);
}

void BumperCar::invert() {
    spline.invert();
}

void BumperCar::setMode(const Mode mode) {
    this->mode = mode;
}

void BumperCar::setSpeed(const float speed) {
    this->speed = speed;
    spline.setSpeed(speed);
}

void BumperCar::shouldDrawPlayer(const bool draw) {
    drawPlayer = draw;
}

void BumperCar::Interface() {
    ImGui::Begin("Bumper Car Debug");
    ImGui::SliderFloat("Cone Radius", &coneRadius, 0.0F, 360.0F);
    ImGui::SliderFloat("Cone Height", &coneHeight, 0.0F, 125.0F);
    ImGui::SliderFloat("Tracking Distance", &trackingDistance, 0.0F, 100.0F);
    ImGui::SliderFloat("Venture Distance", &ventureDistance, 0.0F, 1000.0F);
    ImGui::Checkbox("Paused", &paused);
    ImGui::End();
}

auto BumperCar::hasBroke() const -> bool {
    return isBroken;
}

void BumperCar::setIsPlayer(const bool isPlayer) {
    this->isPlayer = isPlayer;
}

void BumperCar::isCurrentPlayer(const bool isCurrentPlayer) {
    if (isCurrentPlayer) {
        person = Model("../Assets/objects/person-sitting/bodyless.obj");
    } else {
        person = Model("../Assets/objects/person-sitting/person.obj");
    }
}