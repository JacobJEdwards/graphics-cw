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

#include "Config.h"
#include "utils/Shader.h"
#include "utils/ShaderManager.h"

// TODO
// REFACTOR THIS
// should use vertex struct -> create layout 2d -> templated buffer
class InfinitePlane {
public:
    Physics::Attributes attributes;

    InfinitePlane() {
        attributes.mass = 0.0F;
        shader = ShaderManager::Get("Terrain");
        load();
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0F, -0.5F, 0.0F));
    }

    void draw(const glm::mat4 &view, const glm::mat4 &projection,
              const glm::vec3 &lightPos, const glm::vec3 &viewPos, bool depthPass = false) const {

        auto currentShader = depthPass ? ShaderManager::Get("Shadow") : shader;

        buffer.bind();

        if (!depthPass) {
            currentShader->use();
            currentShader->setUniform("light.position", lightPos);
            currentShader->setUniform("light.ambient", glm::vec3(1.0F));
            currentShader->setUniform("light.diffuse", glm::vec3(1.0F));
            currentShader->setUniform("light.specular", glm::vec3(0.5F));
            currentShader->setUniform("viewPos", viewPos);
        }

        currentShader->setUniform("view", view);
        currentShader->setUniform("projection", projection);
        currentShader->setUniform("model", modelMatrix);

        buffer.draw();
        buffer.unbind();

        box.draw(modelMatrix, view, projection);
    }

    void setShader(const std::shared_ptr<Shader> &shader) {
        this->shader = shader;
    }

    [[nodiscard]] auto detectCollisions(const glm::vec3 &position) const -> bool {
        return box.contains(position);
    }

    [[nodiscard]] auto isColliding(const BoundingBox &other) const -> bool {
        return box.isColliding(other);
    }

    [[nodiscard]] auto getBoundingBox() const -> const BoundingBox & {
        return box;
    }

    [[nodiscard]] auto getOffset(const glm::vec3 &point) const -> glm::vec3 {
        return box.getOffset(point);
    }

    [[nodiscard]] auto getOffset(const BoundingBox &other) const -> glm::vec3 {
        return box.getOffset(other);
    }

private:
    static constexpr float SIZE = 1000.0F;

    const std::array<Vertex::Data, 4> vertices = {
            Vertex::Data{glm::vec3(-SIZE, 0.0F, -SIZE), glm::vec2(0.0F, 0.0F)},
            Vertex::Data{glm::vec3(-SIZE, 0.0F, SIZE), glm::vec2(0.0F, 1.0F)},
            Vertex::Data{glm::vec3(SIZE, 0.0F, SIZE), glm::vec2(1.0F, 1.0F)},
            Vertex::Data{glm::vec3(SIZE, 0.0F, -SIZE), glm::vec2(1.0F, 0.0F)}};

    static constexpr std::array<GLuint, 6> indices = {0, 1, 2, 2, 3, 0};

    BoundingBox box{glm::vec3(-SIZE, -1.0F, -SIZE), glm::vec3(SIZE, 1.0F, SIZE)};

    Buffer buffer;

    std::shared_ptr<Shader> shader;

    glm::mat4 modelMatrix = Config::IDENTITY_MATRIX;

    void load() {
        buffer.fill(vertices, indices);

        shader->use();
        shader->setUniform("light.ambient", glm::vec3(0.5F, 0.5F, 0.5F));
        shader->setUniform("light.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
        shader->setUniform("light.specular", glm::vec3(1.0F, 1.0F, 1.0F));
    }
};

#endif // INFINITEPLANE_H
