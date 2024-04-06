//
// Created by Jacob Edwards on 05/04/2024.
//

#include "BumperCar.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
#include <vector>
#include "utils/PlayerManager.h"
#include <iostream>
#include "Entity.h"
#include "App.h"


BumperCar::BumperCar(glm::vec2 centre, float radius, float speed) : Entity(
        "../Assets/objects/bumpercar1/bumper-car.obj"), speed(speed) {
    const int numPoints = 10;

    float angle = 0.0F;
    const float angleIncrement = glm::radians(360.0F / numPoints);

    for (int i = 0; i < numPoints; i++) {
        const float x = centre.x + radius * glm::cos(angle);
        const float z = centre.y + radius * glm::sin(angle);
        points.emplace_back(x, 0.0F, z);
        angle += angleIncrement;
    }

    spline = Physics::Spline(points, Physics::Spline::Type::CATMULLROM, speed);
    attributes.mass = 2.0F;
}

BumperCar::BumperCar(std::vector<glm::vec3> points, float speed) : Entity(
        "../Assets/objects/bumpercar1/bumper-car.obj"), speed(speed),
                                                                   spline(points, Physics::Spline::Type::CATMULLROM,
                                                                          speed), points(points) {
    attributes.mass = 2.0F;
}

void BumperCar::update(float deltaTime) {
    if (pathed) {
        spline.update(deltaTime);
        auto interpolatedPoint = spline.getPoint();
        moveTo(interpolatedPoint);
    }
    if (track) {
        auto player = PlayerManager::GetCurrent();
        auto playerPos = player->getAttributes().position;

        moveTo(playerPos);
    }
    Entity::update(deltaTime);
}

void BumperCar::draw(std::shared_ptr<Shader> shader) const {
    shader->use();
    auto mat = attributes.transform;
    // rotate the car to face the direction it is moving
    mat = glm::rotate(mat, glm::radians(-90.0F), glm::vec3(0.0F, 1.0F, 0.0F));
    shader->setUniform("model", mat);
    model->draw(shader);

    if (App::debug) {
        // draw spline
    }
}

void BumperCar::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    draw(shader);

    if (App::debug) {
        box.draw(attributes.transform, view, projection);
    }
}

void BumperCar::moveTo(glm::vec3 position) {
    auto forceNeeded =
            attributes.calculateForce(position);

    forceNeeded = glm::vec3(forceNeeded.x, 0.0F, forceNeeded.z);

    glm::vec3 rotation = attributes.calculateRotation(position);
    rotation = glm::vec3(0.0F, rotation.y, 0.0F);

    // if (pathed) {
    // rotation.y -= glm::radians(90.0F);
    //}

    attributes.applyRotation(rotation);
    attributes.applyForce(forceNeeded);
}

void BumperCar::invert() {
    spline.invert();
}

void BumperCar::setTrack(bool track) {
    this->track = track;
    this->pathed = !track;
}

void BumperCar::setPathed(bool pathed) {
    this->pathed = pathed;
    this->track = !pathed;
}

void BumperCar::setSpeed(float speed) {
    this->speed = speed;
    spline.setSpeed(speed);
}

