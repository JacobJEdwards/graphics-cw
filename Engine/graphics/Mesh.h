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

struct Material {
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    glm::vec4 emissive;
    float shininess;
};

class Mesh {
public:
    Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices,
         std::vector<Texture::Data> textures, BoundingBox box, Material material);

    void draw(const std::shared_ptr<Shader> &shader) const;

    void draw() const;

    [[nodiscard]] auto getBoundingBox() const -> BoundingBox;

    [[nodiscard]] auto getTextures() const -> const std::vector<Texture::Data> &;

    [[nodiscard]] auto getBuffer() const -> const VertexBuffer &;

    [[nodiscard]] auto getMaterial() const -> Material;

private:
    std::vector<Texture::Data> textures;

    BoundingBox box{glm::vec3(0.0F), glm::vec3(0.0F)};

    std::unique_ptr<VertexBuffer> buffer;

    Material material;
};

#endif // CW_MESH_H
