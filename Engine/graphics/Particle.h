//
// Created by Jacob Edwards on 30/03/2024.
//

#ifndef CW_PARTICLE_H
#define CW_PARTICLE_H

#include "Renderable.h"
#include <algorithm>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <memory>
#include <utility>
#include <vector>
#include "utils/Shader.h"
#include "utils/ShaderManager.h"
#include "utils/Buffer.h"
#include "utils/Vertex.h"
#include "Model.h"
#include <GL/glew.h>
#include <iostream>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float life;

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

    void add(Particle &&particle) {
        particles.push_back(particle);
    }

    void update(float dt) {
        for (auto &particle: particles) {
            particle.position += particle.velocity * dt;
            particle.life -= dt;
        }

        particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle &particle) {
            return particle.life <= 0.0F;
        }), particles.end());
    }

    void draw(std::shared_ptr<Shader> shader) const override {
        buffer->bind();
        shader->use();
        for (const auto &particle: particles) {
            shader->setUniform("offset", particle.position);
            shader->setUniform("color", particle.color);
            shader->setUniform("life", particle.life);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }
        buffer->unbind();
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

    void setup() {
        buffer = std::make_shared<Buffer>();
        shader = ShaderManager::Get("Particle");
        buffer->fill({
                             Vertex::Data(glm::vec3(-0.5F, -0.5F, 0.0F)),
                             Vertex::Data(glm::vec3(0.5F, -0.5F, 0.0F)),
                             Vertex::Data(glm::vec3(0.5F, 0.5F, 0.0F)),
                             Vertex::Data(glm::vec3(-0.5F, 0.5F, 0.0F))
                     }, {
                             0, 1, 2,
                             2, 3, 0
                     });
    }
};

#endif