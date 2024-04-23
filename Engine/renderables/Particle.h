//
// Created by Jacob Edwards on 30/03/2024.
//

#ifndef CW_PARTICLE_H
#define CW_PARTICLE_H

#include "Config.h"
#include "physics/ModelAttributes.h"
#include "renderables/Renderable.h"
#include <array>
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <memory>
#include <mutex>
#include <vector>
#include "graphics/Shader.h"
#include "graphics/buffers/VertexBuffer.h"
#include "graphics/Vertex.h"
#include <GL/glew.h>
#include "graphics/Color.h"
#include "utils/Singleton.h"

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

class ParticleSystem final : public Renderable, public Singleton<ParticleSystem> {
    friend class Singleton;

public:
    using Renderable::draw;

    void add(const Particle &particle);

    void update(float deltaTime);

    void draw(std::shared_ptr<Shader> shader) const override;

    void generate(const Physics::Attributes &attributes, const glm::vec3 &offset = glm::vec3(0.0F),
                  int numParticles = 100,
                  const glm::vec3 &color = Color::WHITE);

    void generate(const glm::vec3 &position, const glm::vec3 &velocity = glm::vec3(0.0F),
                  const glm::vec3 &color = Color::WHITE, int numParticles = 100);

    void interface();

    explicit ParticleSystem(Token) : ParticleSystem() {
    }

private:
    ParticleSystem();

    std::vector<Particle> particles;
    std::shared_ptr<VertexBuffer> buffer;

    int MAX_PARTICLES = 10000;

    float scale = 1.0F;

    const std::array<Vertex::Data, 4> vertices = {
        Vertex::Data{glm::vec3(-0.5F, -0.5F, 0.0F)},
        Vertex::Data{glm::vec3(0.5F, -0.5F, 0.0F)},
        Vertex::Data{glm::vec3(0.5F, 0.5F, 0.0F)},
        Vertex::Data{glm::vec3(-0.5F, 0.5F, 0.0F)}
    };

    const std::array<GLuint, 6> indices = {0, 1, 2, 2, 3, 0};

    bool shouldDraw = true;

    void setup();
};

#endif
