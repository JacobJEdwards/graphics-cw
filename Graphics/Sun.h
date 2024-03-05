//
// Created by Jacob Edwards on 27/02/2024.
//

#ifndef SUN_H
#define SUN_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Model.h"

#include "Shader.h"

class Sun {
public:
    Sun() = default;

    void update(const float dt) {
        angle += 0.5F * dt;
        position.x = 10.0F * cos(angle);
        position.y = 10.0F * sin(angle);
    }

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const {
        // glBindVertexArray(VAO);
        glDepthFunc(GL_LEQUAL);
        shader->use();
        shader->setUniform("view", view);
        shader->setUniform("projection", projection);
        auto model = glm::mat4(1.0F);
        model = glm::translate(model, glm::vec3(position, -10.0F));
        model = glm::scale(model, glm::vec3(scale));
        shader->setUniform("model", model);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // glBindVertexArray(0);
        sun.draw(shader);
        glDepthFunc(GL_LESS);
    }

    void setPosition(const glm::vec2 &pos) {
        Sun::position = pos;
    }

    void setScale(const float newScale) {
        Sun::scale = newScale;
    }

    [[nodiscard]] auto getPosition() const -> glm::vec2 {
        return position;
    }

private:
    Shader *shader = new Shader("../Assets/shaders/sun.vert", "../Assets/shaders/sun.frag");
    Model sun{"../Assets/objects/sun/sun.obj"};
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    glm::vec2 position = glm::vec2(0.0F, 0.0F);
    float scale = 0.001F;
    float angle = 0.0F;

    static constexpr std::array<GLfloat, 20> vertices = {
        -0.5F, -0.5F, 0.0F, 0.0F, 0.0F,
    -0.5F,  0.5F, 0.0F, 0.0F, 1.0F,
     0.5F,  0.5F, 0.0F, 1.0F, 1.0F,
     0.5F, -0.5F, 0.0F, 1.0F, 0.0F
    };

    static constexpr std::array<GLuint, 6> indices = {
        0, 1, 2,
        2, 3, 0
        };

    /*
    void load() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);
    }
    */
};

#endif //SUN_H
