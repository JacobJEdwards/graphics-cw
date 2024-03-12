//
// Created by Jacob Edwards on 27/02/2024.
//

#ifndef SUN_H
#define SUN_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "graphics/Model.h"

#include "utils/Shader.h"

class Sun {
public:
    Sun() = default;

    void update(const float dt) {
        angle += 0.5F * dt;
        position.x = 10.0F * cos(angle);
        position.y = 10.0F * sin(angle);
    }

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const {
        glDepthFunc(GL_LEQUAL);

        shader->use();
        shader->setUniform("view", view);
        shader->setUniform("projection", projection);

        auto model = glm::mat4(1.0F);
        model = glm::translate(model, glm::vec3(position, -10.0F));
        model = glm::scale(model, glm::vec3(scale));
        shader->setUniform("model", model);

        sun.draw(shader);

        glDepthFunc(GL_LESS);
    }

    void setPosition(const glm::vec2 &pos) {
        position = pos;
    }

    void setScale(const float newScale) {
        scale = newScale;
    }

    [[nodiscard]] auto getPosition() const -> glm::vec2 {
        return position;
    }

private:
    Shader *shader = new Shader("../Assets/shaders/sun.vert", "../Assets/shaders/sun.frag");
    Model sun{"../Assets/objects/sun/sun.obj"};

    glm::vec2 position = glm::vec2(0.0F, 0.0F);
    float scale = 0.001F;
    float angle = 0.0F;

};

#endif //SUN_H
