//
// Created by Jacob Edwards on 25/02/2024.
//

#ifndef INFINITEPLANE_H
#define INFINITEPLANE_H
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shader.h"

class InfinitePlane {
public:
    InfinitePlane() {
        load();
    }

    void draw(const glm::mat4 & view, const glm::mat4 & projection ) const {
        glBindVertexArray(VAO);
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        constexpr auto model = glm::mat4(1.0F);
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

    }
private:
    static constexpr float vertices[] = {
        -1.0F, 0.0F, -1.0F,
        -1.0F, 0.0F, 1.0F,
        1.0F, 0.0F, 1.0F,
        1.0F, 0.0F, -1.0F
    };

    static constexpr GLuint indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint VAO, VBO, EBO;

    Shader shader{"../Assets/shaders/infinitePlane.vert", "../Assets/shaders/infinitePlane.frag"};

    void load() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
    }
};



#endif //INFINITEPLANE_H
