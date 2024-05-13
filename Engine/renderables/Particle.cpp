//
// Created by Jacob Edwards on 30/03/2024.
//

#include "Particle.h"

#include <iostream>


#include "Config.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <memory>
#include <print>

#include "physics/ModelAttributes.h"
#include "renderables/Entity.h"
#include "graphics/Shader.h"
#include "utils/ShaderManager.h"
#include "graphics/buffers/VertexBuffer.h"
#include <GL/glew.h>
#include "utils/PlayerManager.h"
#include "graphics/Color.h"
#include "utils/Random.h"
#include <vector>
#include "imgui/imgui.h"

Particle::Particle() : position(0.0F), velocity(0.0F), color(1.0F), life(0.0F) {
}

Particle::Particle(const glm::vec3 position, const glm::vec3 velocity, const glm::vec4 color,
                   const float life, const float scale) : position(position),
                                                          velocity(velocity),
                                                          color(color),
                                                          life(life), scale(scale) {
}

Particle::Particle(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 color,
                   const float life, const float scale) : position
                                                          (position),
                                                          velocity(velocity),
                                                          color(glm::vec4(color, 0.8F)),
                                                          life(life), scale(scale) {
}

ParticleSystem::ParticleSystem() {
    setup();
}


void ParticleSystem::add(const Particle &particle) {
    if (particles.size() >= MAX_PARTICLES) {
        particles.erase(particles.begin());
    }

    particles.push_back(particle);
}

void ParticleSystem::update(const float deltaTime) {
    const auto player = PlayerManager::GetInstance().getCurrent();

    const auto camera = player->getCamera();

    const auto forward = camera.getFront();
    const auto up = camera.getUp();
    const auto right = camera.getRight();

    for (auto &particle: particles) {
        particle.position += particle.velocity * deltaTime;
        particle.life -= deltaTime;

        auto model = Config::IDENTITY_MATRIX;
        model = translate(model, particle.position);
        // rotate model to face camera
        model[0] = glm::vec4(right, 0.0F);
        model[1] = glm::vec4(up, 0.0F);
        model[2] = glm::vec4(-forward, 0.0F);
        model = glm::scale(model, glm::vec3(this->scale * particle.scale));
        particle.model = model;
    }

    std::erase_if(particles, [](const Particle &particle) {
        return particle.life <= 0.0F;
    });
}

void ParticleSystem::draw(const std::shared_ptr<Shader> shader) const {
    if (!shouldDraw) {
        return;
    }
    buffer->bind();

    GLuint previousBlendSrc;
    GLuint previousBlendDst;

    glGetIntegerv(GL_BLEND_SRC, reinterpret_cast<GLint *>(&previousBlendSrc));
    glGetIntegerv(GL_BLEND_DST, reinterpret_cast<GLint *>(&previousBlendDst));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    shader->use();
    for (const auto &particle: particles) {
        shader->setUniform("model", particle.model);
        shader->setUniform("color", particle.color);
        shader->setUniform("life", particle.life);

        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
    }

    buffer->unbind();

    glBlendFunc(previousBlendSrc, previousBlendDst);
}

void ParticleSystem::generate(const Physics::Attributes &attributes, const glm::vec3 &offset, const int numParticles,
                              const glm::vec3 &color) {
    generate(attributes.position + offset, attributes.velocity, color, numParticles);
}

void ParticleSystem::generate(const glm::vec3 &position, const glm::vec3 &velocity, const glm::vec3 &color,
                              const int numParticles, const float life, const float scale) {
    for (int i = 0; i < numParticles; i++) {
        const auto pos = position + Random::Vec3(-3.0F, 3.0F);
        const auto vel = velocity * Random::Float(0.8F, 1.2F);
        const auto col = glm::vec4(color, 0.8F) * Random::Float(0.8F, 1.2F);
        const auto newLife = Random::Float(0.75F, 1.25F) * life;

        add(Particle(pos, vel, col, newLife, scale));
    }
}


void ParticleSystem::setup() {
    buffer = std::make_shared<VertexBuffer>();
    shader = ShaderManager::GetInstance().get("Particle");
    buffer->fill(vertices, indices);
}

void ParticleSystem::interface() {
    ImGui::Begin("Particle System");

    ImGui::Text("Particles: %lu", particles.size());
    ImGui::SliderInt("Max Particles", &MAX_PARTICLES, 0.0F, 100000.0F);
    ImGui::SliderFloat("Scale", &scale, 0.0F, 10.0F);

    ImGui::Checkbox("Draw", &shouldDraw);

    ImGui::End();
}
