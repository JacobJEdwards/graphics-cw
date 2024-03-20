//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_MESH_H
#define CW_MESH_H

#include <GL/glew.h>
#include <vector>

#include "graphics/Texture.h"
#include "utils/BoundingBox.h"
#include "utils/Buffer.h"
#include "utils/Shader.h"
#include "utils/Vertex.h"

class Mesh {
public:
    Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices,
         std::vector<Texture::Data> textures, const BoundingBox &box);

    void draw(const std::shared_ptr<Shader> &shader) const;

    auto detectCollisions(const glm::vec3 &position) const -> bool;

    glm::vec3 getCentre() const;

    glm::vec3 getOffset(const glm::vec3 &point) const;

    glm::vec3 getOffset(const BoundingBox &other) const;

    auto isColliding(const BoundingBox &other) const -> bool;

    auto getBoundingBox() const -> BoundingBox;

    void translate(const glm::vec3 &translation);

    void transform(const glm::mat4 &transformation);

    void scale(const glm::vec3 &scale);

    void rotate(const glm::vec3 &axis, float angle);

private:
    std::vector<Texture::Data> textures;

    BoundingBox box{glm::vec3(0.0F), glm::vec3(0.0F)};

    Buffer buffer;
};

#endif // CW_MESH_H
