//
// Created by Jacob Edwards on 24/02/2024.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <array>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include <span>
#include <string>

#include "graphics/Texture.h"
#include "utils/Buffer.h"
#include "utils/Shader.h"
#include "utils/ShaderManager.h"
#include "utils/Vertex.h"
#include "graphics/Renderable.h"
#include "Sun.h"

constexpr unsigned int NUM_VERTEX = 36;
constexpr unsigned int NUM_FACES = 6;

// TODO
// REFACTOR THIS
class Skybox : public Renderable {
public:
    explicit Skybox(std::span<const std::string, NUM_FACES> faces) {
        buffer.fill(vertices, indices);

        shader = ShaderManager::Get("Sky");
        shader->use();
        shader->setUniform("skybox", 0);

        loadTextures(faces);
    }

    void update(float deltaTime) {
        sun.update(deltaTime);
        time += deltaTime;
    }

    void draw(std::shared_ptr<Shader> shader) const override {
        shader->use();
        shader->setUniform("skybox", 0);

        buffer.bind();
        buffer.draw();
        buffer.unbind();
    }

    [[nodiscard]] auto getSun() -> Sun & {
        return sun;
    }

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override {

        GLenum prevDepthFunc;
        glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&prevDepthFunc));
        glDepthFunc(GL_LEQUAL);

        const auto newView = glm::mat4(glm::mat3(view));

        shader->use();
        shader->setUniform("sunPos", sun.getPosition());
        shader->setUniform("sunDir", sun.getDirection());

        shader->setUniform("time", time);

        shader->setUniform("view", newView);
        shader->setUniform("projection", projection);

        buffer.bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
        buffer.draw();
        buffer.unbind();

        glDepthFunc(prevDepthFunc);
        sun.draw(view, projection);
    }

private:
    Sun sun;

    Texture::Data texture;
    Buffer buffer;

    const std::array<Vertex::Data, NUM_VERTEX> vertices = {
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
            Vertex::Data{glm::vec3(1.0F, -1.0F, 1.0F)}};

    const std::array<GLuint, NUM_VERTEX> indices = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17,
            18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

    void loadTextures(std::span<const std::string, NUM_FACES> faces) {
        texture.id = Texture::Loader::loadCubemap(faces);
        texture.type = Texture::Type::CUBEMAP;
    }

    float time = 0.0F;
};

#endif // SKYBOX_H
