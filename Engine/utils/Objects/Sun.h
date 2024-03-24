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

class Sun {
public:
    Sun() {
        auto shader = ShaderManager::Get("Sun");
        model.setShader(shader);
        auto transform = Config::IDENTITY_MATRIX;
        transform = glm::scale(transform, glm::vec3(scale));
        model.transform(transform);
        model.attributes.position.z = -50.0F; // Sun typically appears farther away
    };

    void update(const float dt) {
        if (!move) {
            return;
        }

        angle += 0.05F * dt;
        float orbitRadius = 100.0F;
        float height = 20.0F;
        float x = orbitRadius * std::cos(angle);
        float y = orbitRadius * std::sin(angle);
        float z = height * std::sin(0.5 * angle);

        model.attributes.position.x = x;
        model.attributes.position.y = y;
        model.attributes.position.z = z;
        model.update(dt);
    }

    void draw(const glm::mat4 &view, const glm::mat4 &projection) {
        glDepthFunc(GL_LEQUAL);

        const auto newView = glm::mat4(glm::mat3(view));
        model.draw(view, projection);

        glDepthFunc(GL_LESS);
    }

    void setPosition(const glm::vec3 &pos) { model.attributes.position = pos; }

    void setScale(const float newScale) { scale = newScale; }

    [[nodiscard]] auto getPosition() const -> glm::vec3 { return model.attributes.position; }

    void interface() {
        ImGui::Begin("Sun");
        ImGui::Checkbox("Update Position", &move);
        ImGui::End();
    }

private:
    Model model = Model("../Assets/objects/sun/sun.obj");

    float scale = 0.01F; // Increase scale to make it more visible
    float angle = 0.0F;
    bool move = true;
};

#endif // SUN_H
