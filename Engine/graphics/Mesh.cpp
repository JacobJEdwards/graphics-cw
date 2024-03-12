//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Mesh.h"

#include <stdexcept>
#include <cstddef>
#include <utility>
#include "utils/Vertex.h"
#include "graphics/Texture.h"
#include "utils/Shader.h"
#include <vector>
#include <string>
#include <GL/glew.h>
#include "utils/Buffer.h"

Mesh::Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices,
           std::vector<Texture::Data> textures) :  textures(std::move(textures)) {

    bufferData = BufferData(std::move(vertices), std::move(indices));
    buffer.fill(bufferData);
}

void Mesh::draw(const Shader *shader) const {
    shader->use();

    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    GLuint normalNr = 1;
    GLuint heightNr = 1;
    GLuint ambientOcclusionNr = 1;
    GLuint emissiveNr = 1;

    for (GLuint i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        const Texture::Type name = textures[i].type;

        switch (name) {
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
                throw std::runtime_error("Unknown texture type");
        }

        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        shader->setUniform(("material.texture_" + toString(name) + number), static_cast<GLint>(i));
    }

    buffer.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(bufferData.indices.size()), GL_UNSIGNED_INT, nullptr);
    buffer.unbind();

    glActiveTexture(GL_TEXTURE0);
}