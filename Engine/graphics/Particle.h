//
// Created by Jacob Edwards on 30/03/2024.
//

#ifndef CW_PARTICLE_H
#define CW_PARTICLE_H

#include "Config.h"
#include "Entity.h"
#include "Renderable.h"
#include <array>
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include <vector>
#include "utils/Shader.h"
#include "utils/Buffer.h"
#include "utils/Vertex.h"
#include <GL/glew.h>
#include "graphics/Color.h"

struct Particle {
    glm::vec3 position = glm::vec3(0.0F);
    glm::vec3 velocity = glm::vec3(0.0F);
    glm::vec4 color = glm::vec4(Color::WHITE, 0.8F);
    glm::mat4 model = Config::IDENTITY_MATRIX;
    float life = 1.0F;

    Particle();

    Particle(glm::vec3 position, glm::vec3 velocity, glm::vec4 color, float life);

    Particle(glm::vec3 position, glm::vec3 velocity, glm::vec3 color, float life);
};

class ParticleSystem final : public Renderable {
public:
    ParticleSystem();

    void add(const Particle &particle);

    void update(float deltaTime);

    void draw(std::shared_ptr<Shader> shader) const override;

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    void draw() const override;

    void generate();

    void generate(const Entity &entity, const glm::vec3 &offset = glm::vec3(0.0F), int numParticles = 100);

private:
    std::vector<Particle> particles;
    std::shared_ptr<Buffer> buffer;

    constexpr static unsigned int MAX_PARTICLES = 10000;


    const std::array<Vertex::Data, 4> vertices = {
        Vertex::Data{glm::vec3(-0.5F, -0.5F, 0.0F)},
        Vertex::Data{glm::vec3(0.5F, -0.5F, 0.0F)},
        Vertex::Data{glm::vec3(0.5F, 0.5F, 0.0F)},
        Vertex::Data{glm::vec3(-0.5F, 0.5F, 0.0F)}
    };

    const std::array<GLuint, 6> indices = {0, 1, 2, 2, 3, 0};

    void setup();
};

#endif
