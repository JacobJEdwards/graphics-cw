//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_MESH_H
#define CW_MESH_H

#include <GL/glew.h>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <vector>

#include "graphics/Texture.h"
#include "utils/BoundingBox.h"
#include "graphics/buffers/VertexBuffer.h"
#include "graphics/Shader.h"
#include "graphics/Vertex.h"

class Mesh {
public:
    Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices,
         std::vector<Texture::Data> textures, BoundingBox box);

    void draw(const std::shared_ptr<Shader> &shader) const;

    void draw() const;

    [[nodiscard]] auto getBoundingBox() const -> BoundingBox;

    [[nodiscard]] auto getTextures() const -> const std::vector<Texture::Data> &;

    [[nodiscard]] auto getBuffer() const -> const VertexBuffer &;

private:
    std::vector<Texture::Data> textures;

    BoundingBox box{glm::vec3(0.0F), glm::vec3(0.0F)};

    std::unique_ptr<VertexBuffer> buffer;
};

#endif // CW_MESH_H
