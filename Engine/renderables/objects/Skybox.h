//
// Created by Jacob Edwards on 24/02/2024.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <array>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>

#include "graphics/buffers/VertexBuffer.h"
#include "graphics/Shader.h"
#include "graphics/Vertex.h"
#include "renderables/Renderable.h"
#include "Sun.h"

constexpr unsigned int NUM_VERTEX = 36;

class Skybox final : public Renderable {
public:
    using Renderable::draw;

    Skybox();

    void update(float deltaTime);

    void draw(std::shared_ptr<Shader> shader) const override;

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    [[nodiscard]] auto getSun() -> Sun &;

private:
    Sun sun;
    std::unique_ptr<VertexBuffer> skyBuffer;

    static constexpr std::array<Vertex::Data, NUM_VERTEX> vertices = {
        Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},
        Vertex::Data{glm::vec3(-1.0F, -1.0F, -1.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, -1.0F)},
        Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},

        Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},
        Vertex::Data{glm::vec3(-1.0F, -1.0F, -1.0F)},
        Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},
        Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},
        Vertex::Data{glm::vec3(-1.0F, 1.0F, 1.0F)},
        Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},

        Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, -1.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},

        Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},
        Vertex::Data{glm::vec3(-1.0F, 1.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, 1.0F)},
        Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},

        Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, -1.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, 1.0F)},
        Vertex::Data{glm::vec3(-1.0F, 1.0F, 1.0F)},
        Vertex::Data{glm::vec3(-1.0F, 1.0F, -1.0F)},

        Vertex::Data{glm::vec3(-1.0F, -1.0F, -1.0F)},
        Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, -1.0F)},
        Vertex::Data{glm::vec3(-1.0F, -1.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, 1.0F)}
    };

    static constexpr std::array<GLuint, NUM_VERTEX> indices = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35
    };
};

#endif // SKYBOX_H
