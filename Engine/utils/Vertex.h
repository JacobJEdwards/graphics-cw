//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_VERTEX_H
#define CW_VERTEX_H

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <vector>

#include "graphics/Texture.h"

namespace Vertex {
    struct Data {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    struct Layout {
        static constexpr GLuint POSITION = 0;
        static constexpr GLuint NORMAL = 1;
        static constexpr GLuint TEX_COORDS = 2;
        static constexpr GLuint TANGENT = 3;
        static constexpr GLuint BITANGENT = 4;
    };

    struct Buffer {
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
    };

    struct BufferData {
        std::vector<Data> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture::Data> textures;
    };
}

#endif //CW_VERTEX_H
