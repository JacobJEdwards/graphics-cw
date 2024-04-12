//
// Created by Jacob Edwards on 24/02/2024.
//

#include "Skybox.h"

#include <GL/glew.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include "Config.h"
#include "Entity.h"
#include "utils/Buffer.h"
#include "utils/Shader.h"
#include "utils/ShaderManager.h"
#include "Sun.h"


Skydome::Skydome() : Entity("../Assets/objects/sphere/sphere.obj") {
    shader = ShaderManager::Get("Sky");
    // scale(glm::vec3(100.0F));
    attributes.gravityAffected = false;
}

void Skydome::update(const float deltaTime) {
    sun.update(deltaTime);
    time += deltaTime;
    Entity::update(deltaTime);
}

void Skydome::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    GLenum prevDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&prevDepthFunc));
    glDepthFunc(GL_LEQUAL);

    const auto newView = glm::mat4(glm::mat3(view));

    shader->use();
    shader->setUniform("sunPos", sun.getPosition());
    shader->setUniform("sunDir", sun.getDirection());

    shader->setUniform("time", time);

    shader->setUniform("view", view);
    shader->setUniform("projection", projection);
    auto mat = Config::IDENTITY_MATRIX;
    //mat = glm::translate(mat, glm::vec3(0.0f, 2.0f, 0.0f));
    mat = glm::scale(mat, glm::vec3(100.0F));
    shader->setUniform("model", mat);

    model->draw(shader);


    glDepthFunc(prevDepthFunc);

    sun.draw(view, projection);
}

[[nodiscard]] auto Skydome::getSun() -> Sun & {
    return sun;
}


// skybox
Skybox::Skybox() {
    buffer = std::make_unique<Buffer>();
    buffer->fill(vertices, indices);

    shader = ShaderManager::Get("Sky");
}

void Skybox::update(const float deltaTime) {
    sun.update(deltaTime);
    time += deltaTime;
}

void Skybox::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();
    buffer->bind();
    buffer->draw();
    buffer->unbind();
}

[[nodiscard]] auto Skybox::getSun() -> Sun & {
    return sun;
}


void Skybox::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    GLenum prevDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&prevDepthFunc));
    glDepthFunc(GL_LEQUAL);

    const auto newView = glm::mat4(glm::mat3(view));

    shader->use();
    shader->setUniform("sunPos", sun.getPosition());
    shader->setUniform("sunDir", sun.getDirection());

    shader->setUniform("time", time);

    shader->setUniform("view", newView);
    shader->setUniform("projection", projection);

    buffer->bind();
    buffer->draw();
    buffer->unbind();

    glDepthFunc(prevDepthFunc);

    sun.draw(view, projection);
}

