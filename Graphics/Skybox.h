//
// Created by Jacob Edwards on 24/02/2024.
//

#ifndef SKYBOX_H
#define SKYBOX_H

#include <GL/glew.h>
#include <cstddef>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "Shader.h"
#include "Texture.h"

constexpr unsigned int NUM_VERTEX = 36;

class Skybox {
public:
    explicit Skybox(std::vector<std::string> faces) : faces(std::move(faces)) {
        init();
        loadTextures();
    }

    explicit Skybox(std::string path) : path(std::move(path)) {
        init();
        loadTextures();
    }

    void draw(const glm::mat4 &projection, const glm::mat4 &view) const {
        glDepthFunc(GL_LEQUAL);
        shader.use();

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTEX);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

private:
    GLuint texture = 0;
    GLuint VAO = 0;
    GLuint VBO = 0;
    std::vector<std::string> faces;
    std::string path;

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

    Shader shader = Shader("../Assets/shaders/skybox.vert", "../Assets/shaders/skybox.frag");

    void init() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

        shader.use();
        shader.setInt("skybox", 0);
    }

    void loadTextures() {
        if (!path.empty()) {
            texture = Texture::Loader::loadCubemap(path);
            return;
        }
        texture = Texture::Loader::loadCubemap(faces);
    }
};


#endif //SKYBOX_H
