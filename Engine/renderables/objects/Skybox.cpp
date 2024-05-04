//
// Created by Jacob Edwards on 24/02/2024.
//

#include "Skybox.h"

#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include "renderables/Entity.h"
#include "graphics/buffers/VertexBuffer.h"
#include "graphics/Shader.h"
#include "utils/ShaderManager.h"
#include "Sun.h"


Skybox::Skybox() {
    skyBuffer = std::make_unique<VertexBuffer>();
    skyBuffer->fill(vertices, indices);

    shader = ShaderManager::GetInstance().get("Sky");
}

void Skybox::update(const float deltaTime) {
    sun.update(deltaTime);
    time += deltaTime;
}

void Skybox::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();
    skyBuffer->bind();
    skyBuffer->draw();
    skyBuffer->unbind();
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

    shader->setUniform("time", time);

    shader->setUniform("view", newView);
    shader->setUniform("projection", projection);

    draw(shader);

    glDepthFunc(prevDepthFunc);

    sun.draw(view, projection);
}

