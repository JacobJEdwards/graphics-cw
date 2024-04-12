//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Mesh.h"

#include "graphics/Texture.h"
#include "utils/BoundingBox.h"
#include "utils/Shader.h"
#include "utils/Vertex.h"
#include <GL/glew.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include "utils/Buffer.h"

Mesh::Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices,
           std::vector<Texture::Data> textures, BoundingBox box)
    : textures(std::move(textures)), box(std::move(box)) {
    buffer = std::make_unique<Buffer>();
    buffer->fill(std::move(vertices), std::move(indices));
}

void Mesh::draw(const std::shared_ptr<Shader> &shader) const {
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    GLuint normalNr = 1;
    GLuint heightNr = 1;
    GLuint ambientOcclusionNr = 1;
    GLuint emissiveNr = 1;

    for (GLuint i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;

        // structured binding
        const auto [id, type, path, target] = textures[i];

        switch (type) {
            case Texture::Type::DIFFUSE:
                number = std::to_string(diffuseNr);
                diffuseNr++;
                break;
            case Texture::Type::SPECULAR:
                number = std::to_string(specularNr);
                specularNr++;
                break;
            case Texture::Type::NORMAL:
                number = std::to_string(normalNr);
                normalNr++;
                break;
            case Texture::Type::HEIGHT:
                number = std::to_string(heightNr);
                heightNr++;
                break;
            case Texture::Type::AMBIENT_OCCLUSION:
                number = std::to_string(ambientOcclusionNr);
                ambientOcclusionNr++;
                break;
            case Texture::Type::EMISSIVE:
                number = std::to_string(emissiveNr);
                emissiveNr++;
                break;
            default:
                std::cerr << "Unknown texture type" << std::endl;
                continue;
        }

        glBindTexture(GL_TEXTURE_2D, id);
        shader->setUniform("material.texture_" + toString(type) + number,
                           static_cast<GLint>(i));
    }

    buffer->bind();
    buffer->draw();
    buffer->unbind();

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::draw() const {
    buffer->bind();
    buffer->draw();
    buffer->unbind();
}

auto Mesh::getBoundingBox() const -> BoundingBox { return box; }
