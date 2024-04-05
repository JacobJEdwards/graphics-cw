//
// Created by Jacob Edwards on 30/03/2024.
//

#ifndef CW_PARTICLE_H
#define CW_PARTICLE_H

#include "Config.h"
#include "Renderable.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/geometric.hpp>
#include <memory>
#include <vector>
#include "utils/Shader.h"
#include "utils/ShaderManager.h"
#include "utils/Buffer.h"
#include "utils/Vertex.h"
#include <GL/glew.h>
#include "utils/PlayerManager.h"
#include <iostream>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    glm::mat4 model;
    float life;

    Particle() : position(0.0F), velocity(0.0F), color(1.0F), life(0.0F) {}

    Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float life) : position(position),
                                                                                    velocity(velocity), color(color),
                                                                                    life(life) {}
};

// instanced rendering
// billboard particles
// particle system
class ParticleSystem : public Renderable {
public:
    ParticleSystem() {
        setup();
    }

    void add(const Particle &particle) {
        particles.push_back(particle);
    }

    void update(float deltaTime) {
        auto player = PlayerManager::GetCurrent();
        auto camera = player->getCamera();
        auto up = camera.getUp();
        auto right = camera.getRight();

        for (auto &particle: particles) {
            particle.position += particle.velocity * deltaTime;
            particle.life -= deltaTime;

            auto model = Config::IDENTITY_MATRIX;
            model = glm::translate(model, particle.position);
            model[0] = glm::vec4(right, 0.0F);
            model[1] = glm::vec4(up, 0.0F);
            model[2] = glm::vec4(glm::cross(right, up), 0.0F);
            model = glm::scale(model, glm::vec3(0.1F));
            particle.model = model;
        }

        particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle &particle) {
            return particle.life <= 0.0F;
        }), particles.end());
    }

    void draw(std::shared_ptr<Shader> shader) const override {
        buffer->bind();

        GLuint previousBlendSrc;
        GLuint previousBlendDst;

        glGetIntegerv(GL_BLEND_SRC, reinterpret_cast<GLint *>(&previousBlendSrc));
        glGetIntegerv(GL_BLEND_DST, reinterpret_cast<GLint *>(&previousBlendDst));

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        // glBlendFunc(GL_ONE, GL_ONE);

        shader->use();
        for (const auto &particle: particles) {
            shader->setUniform("model", particle.model);
            shader->setUniform("color", particle.color);
            shader->setUniform("life", particle.life);

            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
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

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override {
        shader->use();
        shader->setUniform("view", view);
        shader->setUniform("projection", projection);

        draw(shader);
    }

    void draw() const override {
        draw(shader);
    }

private:
    std::vector<Particle> particles;
    std::shared_ptr<Buffer> buffer;

    constexpr static auto MAX_PARTICLES = 10000;


    const std::array<Vertex::Data, 4> vertices = {
            Vertex::Data{glm::vec3(-0.5F, -0.5F, 0.0F)},
            Vertex::Data{glm::vec3(0.5F, -0.5F, 0.0F)},
            Vertex::Data{glm::vec3(0.5F, 0.5F, 0.0F)},
            Vertex::Data{glm::vec3(-0.5F, 0.5F, 0.0F)}};

    const std::array<GLuint, 6> indices = {0, 1, 2, 2, 3, 0};

    void setup() {
        buffer = std::make_shared<Buffer>();
        shader = ShaderManager::Get("Particle");
        buffer->fill(vertices, indices);
    }
};

#endif