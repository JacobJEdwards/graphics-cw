//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_MESH_H
#define CW_MESH_H

#include <vector>
#include <GL/glew.h>

#include "Vertex.h"
#include "Texture.h"
#include "Shader.h"

class Mesh {
public:
    Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices, std::vector<Texture::Data> textures);

    void draw(const Shader *shader) const;

private:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    std::vector<Vertex::Data> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture::Data> textures;

    void setupMesh();
};


#endif //CW_MESH_H
