//
// Created by Jacob Edwards on 26/04/2024.
//

#include "Trees.h"

#include <iostream>
#include <memory>
#include <vector>
#include "graphics/Model.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <span>
#include <GL/glew.h>
#include <utils/ShaderManager.h>
#include "utils/BoundingBox.h"
#include "utils/Random.h"

Trees::Trees() : model("../Assets/objects/tree/tree.obj") {
    shader = ShaderManager::GetInstance().get("Tree");
}

void Trees::generateTrees(const std::span<const glm::vec3> positions) {
    trees.insert(trees.end(), positions.begin(), positions.end());
    setupInstanceData();

    // bounding boxes
    for (const auto &tree: trees) {
        BoundingBox box(tree - glm::vec3(0.5F), tree + glm::vec3(0.5F));
        boundingBoxes.push_back(box);
    }
}

void Trees::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();

    for (const auto &mesh: model.getMeshes()) {
        // set and bind textures
        for (int i = 0; i < mesh->getTextures().size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, mesh->getTextures()[i].id);
            shader->setUniform("texture_diffuse" + std::to_string(i), i);
        }

        glBindVertexArray(mesh->getBuffer().VAO);
        glDrawElementsInstanced(GL_TRIANGLES, mesh->getBuffer().data.indices.size(), GL_UNSIGNED_INT, nullptr,
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

void Trees::setupInstanceData() {
    std::vector<glm::mat4> modelMatrices;
    for (const auto &tree: trees) {
        auto model = glm::mat4(1.0F);
        model = glm::translate(model, tree);
        const float scale = Random::Float(0.08F, 0.12F);
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
        glEnableVertexAttribArray(6); // Change to attribute position 6
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), nullptr); // Change position to 6
        glEnableVertexAttribArray(7); // Increment to the next attribute position
        glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              reinterpret_cast<void *>(sizeof(glm::vec4))); // Increment position to 7
        glEnableVertexAttribArray(8); // Increment to the next attribute position
        glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              reinterpret_cast<void *>(2 * sizeof(glm::vec4))); // Increment position to 8
        glEnableVertexAttribArray(9); // Increment to the next attribute position
        glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              reinterpret_cast<void *>(3 * sizeof(glm::vec4))); // Increment position to 9

        glVertexAttribDivisor(6, 1); // Change to attribute position 6
        glVertexAttribDivisor(7, 1); // Increment to the next attribute position
        glVertexAttribDivisor(8, 1); // Increment to the next attribute position
        glVertexAttribDivisor(9, 1); // Increment to the next attribute position

        glBindVertexArray(0);
    }
}

auto Trees::getBoundingBoxes() const -> const std::vector<BoundingBox> & {
    return boundingBoxes;
}
