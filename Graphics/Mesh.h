//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_MESH_H
#define CW_MESH_H

#include <utility>
#include <vector>
#include <string>
#include <GL/glew.h>

#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"

class Mesh {
public:
    Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices, std::vector<Texture::Data> textures) : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures)) {
        setupMesh();
    }

    void draw(const Shader &shader) const {
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;
        GLuint normalNr = 1;
        GLuint heightNr = 1;

        for (GLuint i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);

            std::string number;
            const Texture::Type name = textures[i].type;

            switch (name) {
                case Texture::Type::DIFFUSE:
                    diffuseNr++;
                    number = std::to_string(diffuseNr);
                    break;
                case Texture::Type::SPECULAR:
                    specularNr++;
                    number = std::to_string(specularNr);
                    break;
                case Texture::Type::NORMAL:
                    normalNr++;
                    number = std::to_string(normalNr);
                    break;
                case Texture::Type::HEIGHT:
                    heightNr++;
                    number = std::to_string(heightNr);
                    break;
            }

            shader.setFloat(("material.texture_" + toString(name) + number), static_cast<float>(i));
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);
    }
private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    std::vector<Vertex::Data> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture::Data> textures;

    void setupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex::Data)), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(), GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(Vertex::Layout::POSITION);
        glVertexAttribPointer(Vertex::Layout::POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data), nullptr);

        // vertex normals
        glEnableVertexAttribArray(Vertex::Layout::NORMAL);
        glVertexAttribPointer(Vertex::Layout::NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data), reinterpret_cast<void*>(offsetof(Vertex::Data, normal)));

        // vertex texture coords
        glEnableVertexAttribArray(Vertex::Layout::TEX_COORDS);
        glVertexAttribPointer(Vertex::Layout::TEX_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data), reinterpret_cast<void*>(offsetof(Vertex::Data, texCoords)));

        // vertex tangent
        glEnableVertexAttribArray(Vertex::Layout::TANGENT);
        glVertexAttribPointer(Vertex::Layout::TANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data), reinterpret_cast<void*>(offsetof(Vertex::Data, tangent)));

        // vertex bitangent
        glEnableVertexAttribArray(Vertex::Layout::BITANGENT);
        glVertexAttribPointer(Vertex::Layout::BITANGENT, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex::Data), reinterpret_cast<void*>(offsetof(Vertex::Data, bitangent)));

        glBindVertexArray(0);
    }
};


#endif //CW_MESH_H
