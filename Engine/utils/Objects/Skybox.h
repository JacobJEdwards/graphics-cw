//
// Created by Jacob Edwards on 24/02/2024.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <array>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <span>
#include <string>

#include "Config.h"
#include "Entity.h"
#include "utils/Buffer.h"
#include "utils/Shader.h"
#include "utils/ShaderManager.h"
#include "utils/Vertex.h"
#include "graphics/Renderable.h"
#include "Sun.h"
#include <iostream>

constexpr unsigned int NUM_VERTEX = 36;
constexpr unsigned int NUM_FACES = 6;

class Skydome : public Entity {
public:
    Skydome() : Entity("../Assets/objects/sphere/sphere.obj") {
        shader = ShaderManager::Get("Sky");
        // scale(glm::vec3(100.0F));
        attributes.gravityAffected = false;
    };

    void update(float deltaTime) override {
        sun.update(deltaTime);
        time += deltaTime;
        Entity::update(deltaTime);
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

        shader->setUniform("view", view);
        shader->setUniform("projection", projection);
        auto mat = Config::IDENTITY_MATRIX;
        //mat = glm::translate(mat, glm::vec3(0.0f, 2.0f, 0.0f));
        mat = glm::scale(mat, glm::vec3(100.0F));
        shader->setUniform("model", mat);

        model->draw(shader);


        glDepthFunc(prevDepthFunc);

        sun.draw(view, projection);
    }

    [[nodiscard]] auto getSun() -> Sun & {
        return sun;
    }

private:
    float time = 0.0F;

    Sun sun;
};

class Skybox : public Renderable {
public:
    explicit Skybox(std::span<const std::string, NUM_FACES> faces) {
        buffer = std::make_unique<Buffer>();
        buffer->fill(vertices, indices);

        shader = ShaderManager::Get("Sky");
    }

    void update(float deltaTime) {
        sun.update(deltaTime);
        time += deltaTime;
    }

    void draw(std::shared_ptr<Shader> shader) const override {
        shader->use();
        buffer->bind();
        buffer->draw();
        buffer->unbind();
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

        buffer->bind();
        buffer->draw();
        buffer->unbind();

        glDepthFunc(prevDepthFunc);

        sun.draw(view, projection);
    }

private:
    Sun sun;
    Moon moon;

    std::unique_ptr<Buffer> buffer;

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

    float time = 0.0F;
};

#endif // SKYBOX_H
