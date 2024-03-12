//
// Created by Jacob Edwards on 24/02/2024.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <array>
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <string>
#include <span>

#include "utils/Shader.h"
#include "graphics/Texture.h"
#include "utils/Buffer.h"

constexpr unsigned int NUM_VERTEX = 36;
constexpr unsigned int NUM_FACES = 6;

// TODO
// REFACTOR THIS
class Skybox {
public:
    explicit Skybox(std::span<const std::string, NUM_FACES> faces)  {
        init();
        loadTextures(faces);
    }

    void draw(const glm::mat4 &projection, const glm::mat4 &view, const GLfloat y) const {
        glDepthFunc(GL_LEQUAL);
        shader->use();

        shader->setUniform("view", view);
        shader->setUniform("projection", projection);
        shader->setUniform("intensity", y);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id);
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTEX);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

private:
    Texture::Data texture;
    Buffer buffer;

    GLuint VAO = 0;
    GLuint VBO = 0;

    const std::vector<Vertex::Data> vertices = {
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
    }

    static constexpr std::array<GLfloat, static_cast<size_t>(NUM_VERTEX * 3)> vertices = {
        // positions
        -1.0F, 1.0F, -1.0F,
        -1.0F, -1.0F, -1.0F,
        1.0F, -1.0F, -1.0F,
        1.0F, -1.0F, -1.0F,
        1.0F, 1.0F, -1.0F,
        -1.0F, 1.0F, -1.0F,

        -1.0F, -1.0F, 1.0F,
        -1.0F, -1.0F, -1.0F,
        -1.0F, 1.0F, -1.0F,
        -1.0F, 1.0F, -1.0F,
        -1.0F, 1.0F, 1.0F,
        -1.0F, -1.0F, 1.0F,

        1.0F, -1.0F, -1.0F,
        1.0F, -1.0F, 1.0F,
        1.0F, 1.0F, 1.0F,
        1.0F, 1.0F, 1.0F,
        1.0F, 1.0F, -1.0F,
        1.0F, -1.0F, -1.0F,

        -1.0F, -1.0F, 1.0F,
        -1.0F, 1.0F, 1.0F,
        1.0F, 1.0F, 1.0F,
        1.0F, 1.0F, 1.0F,
        1.0F, -1.0F, 1.0F,
        -1.0F, -1.0F, 1.0F,

        -1.0F, 1.0F, -1.0F,
        1.0F, 1.0F, -1.0F,
        1.0F, 1.0F, 1.0F,
        1.0F, 1.0F, 1.0F,
        -1.0F, 1.0F, 1.0F,
        -1.0F, 1.0F, -1.0F,

        -1.0F, -1.0F, -1.0F,
        -1.0F, -1.0F, 1.0F,
        1.0F, -1.0F, -1.0F,
        1.0F, -1.0F, -1.0F,
        -1.0F, -1.0F, 1.0F,
        1.0F, -1.0F, 1.0F
    };

    Shader *shader = new Shader("../Assets/shaders/skybox.vert", "../Assets/shaders/skybox.frag");

    void init() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        shader->use();
        shader->setUniform("skybox", 0);
    }

    void loadTextures(std::span<const std::string, NUM_FACES> faces) {
        texture.id = Texture::Loader::loadCubemap(faces);;
        texture.type = Texture::Type::CUBEMAP;
    }
};


#endif //SKYBOX_H
