//
// Created by Jacob Edwards on 27/02/2024.
//

#ifndef SUN_H
#define SUN_H

#include "Config.h"
#include "graphics/Model.h"
#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "utils/Shader.h"
#include "utils/ShaderManager.h"
#include "imgui/imgui.h"
#include "Entity.h"

class Sun : public Entity {
public:
    Sun() : Entity("../Assets/objects/sun/sun.obj") {
        shader = ShaderManager::Get("Sun");

        auto transform = Config::IDENTITY_MATRIX;
        transform = glm::scale(transform, glm::vec3(scale));

        Entity::transform(transform);

        attributes.position.x = 0.0F;
        attributes.position.y = 0.0F;
        attributes.position.z = -50.0F;
    };

    void update(float deltaTime) override {

        if (!move) {
            return;
        }

        attributes.update(deltaTime * 10);

        angle += 0.05F * deltaTime * 10;
        const float orbitRadius = 100.0F;
        const float height = 20.0F;
        const float x = orbitRadius * std::cos(angle);
        const float y = orbitRadius * std::sin(angle);
        const float z = height * std::sin(0.5 * angle);


        attributes.position.x = x;
        attributes.position.y = y;
        attributes.position.z = z;

        // if y pos near horizon, increase scale
        if (y < 1.0F) {
            scale = scale + 0.0001F;
        } else {
            scale = 0.005F;
        }


    }

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override {

        GLenum prevDepthFunc;
        glGetIntegerv(GL_DEPTH_FUNC, reinterpret_cast<GLint *>(&prevDepthFunc));

        glDepthFunc(GL_LEQUAL);

        const auto newView = glm::mat4(glm::mat3(view));

        Entity::draw(newView, projection);

        glDepthFunc(prevDepthFunc);
    }

    void setPosition(const glm::vec3 &pos) { attributes.position = pos; }

    [[nodiscard]] auto getDirection() const -> glm::vec3 { return glm::normalize(attributes.position); }

    void setScale(const float newScale) { scale = newScale; }

    [[nodiscard]] auto getPosition() const -> glm::vec3 { return attributes.position; }

    void interface() {
        ImGui::Begin("Sun");
        ImGui::Checkbox("Update Position", &move);
        ImGui::End();
    }

private:
    float scale = 0.005F;
    float angle = 0.0F;
    bool move = true;
};

#endif // SUN_H
