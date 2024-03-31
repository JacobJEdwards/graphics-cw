//
// Created by Jacob Edwards on 25/02/2024.
//

#ifndef INFINITEPLANE_H
#define INFINITEPLANE_H

#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"
#include "utils/Buffer.h"
#include "utils/Vertex.h"
#include <GL/glew.h>
#include <array>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <memory>

#include "Config.h"
#include "utils/Shader.h"
#include "utils/ShaderManager.h"
#include "ImmovableObject.h"

// TODO
// REFACTOR THIS
// should use vertex struct -> create layout 2d -> templated buffer
class InfinitePlane : public ImmovableObject {
public:
    InfinitePlane() {
        buffer = std::make_unique<Buffer>();
        attributes.mass = 0.0F;
        shader = ShaderManager::Get("Terrain");
        normal = glm::vec3(0.0F, 1.0F, 0.0F);
        box = BoundingBox(glm::vec3(-SIZE, -1.0F, -SIZE), glm::vec3(SIZE, 0.0F, SIZE));
        buffer->fill(vertices, indices);
    }

    void draw(std::shared_ptr<Shader> shader) const override {
        shader->use();
        shader->setUniform("model", attributes.transform);

        buffer->bind();
        buffer->draw();
        buffer->unbind();
    }

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override {
        shader->use();

        shader->setUniform("light.ambient", glm::vec3(1.0F));
        shader->setUniform("light.diffuse", glm::vec3(1.0F));
        shader->setUniform("light.specular", glm::vec3(0.5F));

        shader->setUniform("view", view);
        shader->setUniform("projection", projection);
        shader->setUniform("model", attributes.transform);

        buffer->bind();
        buffer->draw();
        buffer->unbind();

        if (App::debug) {
            box.draw(attributes.transform, view, projection);
        }
    }

private:
    static constexpr float SIZE = 1000.0F;

    const std::array<Vertex::Data, 4> vertices = {
            Vertex::Data{glm::vec3(-SIZE, 0.0F, -SIZE), glm::vec2(0.0F, 0.0F)},
            Vertex::Data{glm::vec3(-SIZE, 0.0F, SIZE), glm::vec2(0.0F, 1.0F)},
            Vertex::Data{glm::vec3(SIZE, 0.0F, SIZE), glm::vec2(1.0F, 1.0F)},
            Vertex::Data{glm::vec3(SIZE, 0.0F, -SIZE), glm::vec2(1.0F, 0.0F)}};

    static constexpr std::array<GLuint, 6> indices = {0, 1, 2, 2, 3, 0};

    std::unique_ptr<Buffer> buffer;
};

#endif // INFINITEPLANE_H
