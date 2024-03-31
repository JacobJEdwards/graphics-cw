//
// Created by Jacob Edwards on 30/03/2024.
//

#ifndef CW_PARTICLE_H
#define CW_PARTICLE_H

#include "Renderable.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "utils/ShaderManager.h"
#include "utils/Buffer.h"

struct Particle : public Renderable {
    glm::vec3 position = glm::vec3(0.0F);
    glm::vec3 velocity = glm::vec3(0.0F);
    glm::vec4 color = glm::vec4(1.0F);
    float life = 0.0F;

    Particle() {
        buffer = std::make_unique<Buffer>();
        shader = ShaderManager::Get("Particle");
        buffer->fill(vertices, indices);
    };

    void update(float deltaTime) {
        life -= deltaTime;
        position += velocity * deltaTime;
    };

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override {
        buffer->bind();

        shader->use();
        shader->setUniform("view", view);
        shader->setUniform("projection", projection);
        shader->setUniform("model", glm::translate(glm::mat4(1.0F), position));
        shader->setUniform("color", color);
        shader->setUniform("life", life);

        buffer->draw();
        buffer->unbind();
    };
private:
    std::unique_ptr<Buffer> buffer;

    const std::vector<Vertex::Data> vertices = {
            Vertex::Data(glm::vec3(-0.5F, -0.5F, 0.0F), glm::vec2(0.0F, 0.0F)),
            Vertex::Data(glm::vec3(0.5F, -0.5F, 0.0F), glm::vec2(1.0F, 0.0F)),
            Vertex::Data(glm::vec3(0.5F, 0.5F, 0.0F), glm::vec2(1.0F, 1.0F)),
            Vertex::Data(glm::vec3(-0.5F, 0.5F, 0.0F), glm::vec2(0.0F, 1.0F))
    };

    const std::vector<GLuint> indices = {
            0, 1, 2,
            2, 3, 0
    };

};


#endif //CW_PARTICLE_H
