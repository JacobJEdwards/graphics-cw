//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "utils/BoundingBox.h"

#include "utils/Shader.h"
#include "Config.h"
#include "graphics/Model.h"

class Player {
public:
    Player() = default;

    [[nodiscard]] auto getPosition() const -> glm::vec3 {
        return position;
    }

    void setPosition(const glm::vec3 &position) {
        Player::position = position;
    }


    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &position, const float yaw) const {
        shader.use();
        shader.setUniform("view", view);
        shader.setUniform("projection", projection);
        auto model = Config::IDENTITY_MATRIX;
        model = glm::translate(model, position);
        model = glm::scale(model, size);
        model = glm::translate(model, glm::vec3(0.0F, -7.0F, 0.0F));
        // translate back a bit
        model = glm::translate(model, glm::vec3(0.0F, 0.0F, -1.3F));

        // rotate obj to face direction
        model = glm::rotate(model, glm::radians(-yaw), glm::vec3(0.0F, 1.0F, 0.0F));
        model = glm::rotate(model, glm::radians(90.0F), glm::vec3(0.0F, 1.0F, 0.0F));


        shader.setUniform("model", model);
        player.draw(&shader);
    }


private:
    glm::vec3 position;
    glm::vec3 size = glm::vec3(0.05F);

    Shader shader = Shader("../Assets/shaders/base.vert", "../Assets/shaders/base.frag");
    Model player = Model("../Assets/objects/person/person.obj");
};



#endif //PLAYER_H
