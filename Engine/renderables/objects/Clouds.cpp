//
// Created by Jacob Edwards on 08/05/2024.
//
/*
 * https://learnopengl.com/Advanced-OpenGL/Instancing
 */

#include "Clouds.h"

#include <memory>
#include <string>
#include <vector>
#include "graphics/Model.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <span>
#include <GL/glew.h>
#include <utils/ShaderManager.h>
#include "graphics/Shader.h"
#include "graphics/Texture.h"
#include "utils/BoundingBox.h"
#include "utils/Random.h"

Clouds::Clouds() : model("../Assets/objects/clouds/cloud.obj") {
    shader = ShaderManager::GetInstance().get("Cloud");
}

void Clouds::generateClouds(const std::span<const glm::vec3> positions) {
    trees.insert(trees.end(), positions.begin(), positions.end());
    setupInstanceData();

    // bounding boxes
    for (const auto &tree: trees) {
        const BoundingBox box(tree - glm::vec3(0.5F), tree + glm::vec3(0.5F));
        boundingBoxes.push_back(box);
    }
}

void Clouds::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();

    for (const auto &mesh: model.getMeshes()) {
        // set and bind textures
        for (int i = 0; i < mesh->getTextures().size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, mesh->getTextures()[i].id);
            const auto type = mesh->getTextures()[i].type;
            const std::string typeName = Texture::toString(type);
            shader->setUniform("material.texture_" + typeName + std::to_string(i + 1), i);
        }
        const auto &[ambient, diffuse, specular, emissive, shininess] = mesh->getMaterial();
        shader->setUniform("material.ambient", ambient);
        shader->setUniform("material.diffuse", diffuse);
        shader->setUniform("material.specular", specular);
        shader->setUniform("material.emissive", emissive);
        shader->setUniform("material.shininess", shininess);

        glBindVertexArray(mesh->getBuffer().VAO);
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(mesh->getBuffer().data.indices.size()),
                                GL_UNSIGNED_INT,
                                nullptr,
                                static_cast<GLsizei>(trees.size()));
        glBindVertexArray(0);
    }

    /*
    for (const auto pos : trees) {
        glm::mat4 mat = glm::mat4(1.0F);
        mat = glm::translate(mat, pos);
        mat = glm::scale(mat, glm::vec3(0.5F));
        mat = glm::rotate(mat, Random::Float(0.0F, 360.0F), glm::vec3(0.0F, 1.0F, 0.0F));
        shader->setUniform("model", mat);
        model.draw(shader);
    }
    */
}

void Clouds::setupInstanceData() {
    std::vector<glm::mat4> modelMatrices;
    for (const auto &tree: trees) {
        auto model = glm::mat4(1.0F);
        model = glm::translate(model, tree);
        const float scale = Random::Float(0.05F, 0.20F);
        model = glm::scale(model, glm::vec3(scale));
        model = glm::rotate(model, Random::Float(0.0F, 360.0F), glm::vec3(0.0F, 1.0F, 0.0F));
        modelMatrices.push_back(model);
    }


    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(modelMatrices.size() * sizeof(glm::mat4)),
                 modelMatrices.data(), GL_STATIC_DRAW);

    for (const auto &mesh: model.getMeshes()) {
        const GLuint VAO = mesh->getBuffer().VAO;
        glBindVertexArray(VAO);
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), nullptr); // Change position to 6
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              reinterpret_cast<void *>(sizeof(glm::vec4))); // Increment position to 7
        glEnableVertexAttribArray(8);
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              reinterpret_cast<void *>(2 * sizeof(glm::vec4))); // Increment position to 8
        glEnableVertexAttribArray(9);
        glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              reinterpret_cast<void *>(3 * sizeof(glm::vec4))); // Increment position to 9

        glVertexAttribDivisor(6, 1);
        glVertexAttribDivisor(7, 1);
        glVertexAttribDivisor(8, 1);
        glVertexAttribDivisor(9, 1);

        glBindVertexArray(0);
    }
}

auto Clouds::getBoundingBoxes() const -> const std::vector<BoundingBox> & {
    return boundingBoxes;
}
