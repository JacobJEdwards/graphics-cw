//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_VERTEX_H
#define CW_VERTEX_H

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <GL/glew.h>

namespace Vertex {
    struct Data {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    struct Data2D {
        glm::vec2 position;
        glm::vec2 texCoords;
    };

    struct Layout {
        static constexpr GLuint POSITION = 0;
        static constexpr GLuint NORMAL = 1;
        static constexpr GLuint TEX_COORDS = 2;
        static constexpr GLuint TANGENT = 3;
        static constexpr GLuint BITANGENT = 4;
    };

    struct Layout2D {
        static constexpr GLuint POSITION = 0;
        static constexpr GLuint TEX_COORDS = 1;
    };
}

#endif //CW_VERTEX_H
