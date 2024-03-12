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

class Mesh {
public:
    Mesh(std::vector<Vertex::Data> vertices, std::vector<GLuint> indices, std::vector<Texture::Data> textures);

    void draw(const Shader *shader) const;

private:
    Buffer buffer;
    BufferData bufferData;

    std::vector<Texture::Data> textures;
};


#endif //CW_MESH_H
