//
// Created by Jacob Edwards on 30/03/2024.
//

#include "Particle.h"


#include "Config.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <memory>
#include "Entity.h"
#include "utils/Shader.h"
#include "utils/ShaderManager.h"
#include "utils/Buffer.h"
#include <GL/glew.h>
#include "utils/PlayerManager.h"
#include "graphics/Color.h"
#include "utils/Random.h"
#include <vector>

Particle::Particle() : position(0.0F), velocity(0.0F), color(1.0F), life(0.0F) {
}

Particle::Particle(const glm::vec3 position, const glm::vec3 velocity, const glm::vec4 color,
                   const float life) : position
                                       (position),
                                       velocity(velocity),
                                       color(color),
                                       life(life) {
}

Particle::Particle(const glm::vec3 position, const glm::vec3 velocity, const glm::vec3 color,
                   const float life) : position
                                       (position),
                                       velocity(velocity),
                                       color(glm::vec4(color, 0.8F)),
                                       life(life) {
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
    const auto player = PlayerManager::GetCurrent();
    const auto camera = player->getCamera();
    const auto up = camera.getUp();
    const auto right = camera.getRight();

    for (auto &particle: particles) {
        particle.position += particle.velocity * deltaTime;
        particle.life -= deltaTime;

        auto model = Config::IDENTITY_MATRIX;
        model = translate(model, particle.position);
        model[0] = glm::vec4(right, 0.0F);
        model[1] = glm::vec4(up, 0.0F);
        model[2] = glm::vec4(glm::cross(right, up), 0.0F);
        model = scale(model, glm::vec3(0.1F));
        particle.model = model;
    }

    std::erase_if(particles, [](const Particle &particle) {
        return particle.life <= 0.0F;
    });
}

void ParticleSystem::draw(const std::shared_ptr<Shader> shader) const {
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

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    }

    /*
    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<void *>(offsetof(Particle, position)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<void *>(offsetof(Particle, velocity)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<void *>(offsetof(Particle, color)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<void *>(offsetof(Particle, life)));
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<void *>(offsetof(Particle, model)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glVertexAttribDivisor(7, 1);

    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr,
                            particles.size());

    glDeleteBuffers(1, &instanceVBO);
     */

    buffer->unbind();

    glBlendFunc(previousBlendSrc, previousBlendDst);
}

void ParticleSystem::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);

    draw(shader);
}

void ParticleSystem::draw() const {
    draw(shader);
}

void ParticleSystem::generate() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        constexpr auto position = glm::vec3(0.0F);
        constexpr auto velocity = glm::vec3(0.0F);
        constexpr auto color = glm::vec4(Color::WHITE, 0.8F);
        constexpr auto life = 0.0F;

        add(Particle(position, velocity, color, life));
    }
}

void ParticleSystem::generate(const Entity &entity, const glm::vec3 &offset, const int numParticles) {
    for (int i = 0; i < numParticles; i++) {
        const auto position = entity.attributes.position + offset + Random::Vec3(-1.0F, 1.0F);
        const auto velocity = entity.attributes.velocity + Random::Vec3(-1.0F, 1.0F);
        const auto color = glm::vec4(Color::SILVER, 0.8F) * Random::Float(0.8F, 1.2F);
        const auto life = Random::Float(0.8F, 1.0F);

        add(Particle(position, velocity, color, life));
    }
}

void ParticleSystem::setup() {
    buffer = std::make_shared<Buffer>();
    shader = ShaderManager::Get("Particle");
    buffer->fill(vertices, indices);
}
