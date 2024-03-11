//
// Created by Jacob Edwards on 25/02/2024.
//

#ifndef INFINITEPLANE_H
#define INFINITEPLANE_H
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../Shader.h"

class InfinitePlane {
public:
    InfinitePlane() {
        load();
    }

    void draw(const glm::mat4 & view, const glm::mat4 & projection, const glm::vec3& lightPos, const glm::vec3 &viewPos ) const {
        glBindVertexArray(VAO);
        shader.use();
        shader.setUniform("view", view);
        shader.setUniform("projection", projection);
        shader.setUniform("light.position", lightPos);
        shader.setUniform("viewPos", viewPos);
        auto model = glm::mat4(1.0F);
        // move model down by 2.0F
        model = glm::translate(model, glm::vec3(0.0F, -0.5F, 0.0F));
        shader.setUniform("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);

    }
private:
    static constexpr float SIZE = 1000.0F;

    static constexpr std::array<GLfloat, 20> vertices = {
        -SIZE, 0.0F, -SIZE, 0.0F, 0.0F,
        -SIZE, 0.0F, SIZE, 0.0F, 1.0F,
        SIZE, 0.0F, SIZE, 1.0F, 1.0F,
        SIZE, 0.0F, -SIZE, 1.0F, 0.0F
    };

    static constexpr std::array<GLuint, 6> indices = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

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

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        shader.use();
        shader.setUniform("light.ambient", glm::vec3(0.5F, 0.5F, 0.5F));
        shader.setUniform("light.diffuse", glm::vec3(0.5F, 0.5F, 0.5F));
        shader.setUniform("light.specular", glm::vec3(1.0F, 1.0F, 1.0F));
    }
};



#endif //INFINITEPLANE_H
