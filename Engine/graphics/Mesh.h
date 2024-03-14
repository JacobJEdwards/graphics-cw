//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_MESH_H
#define CW_MESH_H

#include <vector>
#include <GL/glew.h>

#include "utils/Vertex.h"
#include "graphics/Texture.h"
#include "utils/Shader.h"
#include "utils/Buffer.h"
#include "utils/BoundingBox.h"


class Mesh {
public:
    Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices, std::vector<Texture::Data> textures, BoundingBox box);

    void draw(const Shader *shader) const;
    auto detectCollisions(const glm::vec3 &position) const -> bool;
    glm::vec3 getCentre() const;
    glm::vec3 getOffset(const glm::vec3 &point) const;

private:
    std::vector<Texture::Data> textures;

    BoundingBox box{ glm::vec3(0.0F), glm::vec3(0.0F) };

    Buffer buffer;
};


#endif //CW_MESH_H
