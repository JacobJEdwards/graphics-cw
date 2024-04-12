//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_VERTEX_H
#define CW_VERTEX_H

#include <GL/glew.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>

namespace Vertex {
    struct Data {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 tangent;
        glm::vec3 bitangent;

        Data() = default;

        explicit Data(const glm::vec3 &position)
                : position(position), normal(glm::vec3(0.0F)), texCoords(glm::vec2(0.0F)), tangent(glm::vec3(0.0F)),
                  bitangent(glm::vec3(0.0F)) {
        }

        Data(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &texCoords, const glm::vec3 &tangent,
             const glm::vec3 &bitangent)
                : position(position), normal(normal), texCoords(texCoords), tangent(tangent), bitangent(bitangent) {
        }

        Data(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &texCoords)
                : position(position), normal(normal), texCoords(texCoords), tangent(glm::vec3(0.0F)),
                  bitangent(glm::vec3(0.0F)) {
        }

        Data(const glm::vec3 &postion, const glm::vec2 &texCoords)
                : position(postion), normal(glm::vec3(0.0F)), texCoords(texCoords), tangent(glm::vec3(0.0F)),
                  bitangent(glm::vec3(0.0F)) {
        }

        //
    };

    struct Data2D {
        glm::vec2 position;
        glm::vec2 texCoords;
    };

    struct Layout {
        static constexpr GLuint POSITION = 0U;
        static constexpr GLuint NORMAL = 1U;
        static constexpr GLuint TEX_COORDS = 2U;
        static constexpr GLuint TANGENT = 3U;
        static constexpr GLuint BITANGENT = 4U;
    };

    struct Layout2D {
        static constexpr GLuint POSITION = 0U;
        static constexpr GLuint TEX_COORDS = 1U;
    };
}

#endif // CW_VERTEX_H
