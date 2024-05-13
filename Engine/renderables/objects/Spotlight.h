//
// Created by Jacob Edwards on 12/05/2024.
//

#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include "utils/Lights.h"
#include "graphics/Model.h"
#include "utils/ShaderManager.h"
#include "renderables/Renderable.h"

class SpotlightObject final : public Renderable {
public:
    using Renderable::draw;

    SpotlightObject() : lightModel("../Assets/objects/spotlight/spotlight.obj") {
        shader = ShaderManager::GetInstance().get("Base");

        setupLight();
    }

    void draw(const std::shared_ptr<Shader> shader) const override {
        shader->use();
        shader->setUniform("model", transform);
        lightModel.draw(shader);
    }

    [[nodiscard]] auto getSpotLight() const -> const SpotLight & {
        return spotLight;
    }

private:
    SpotLight spotLight;
    Model lightModel;
    glm::mat4 transform;

    void setupLight() {
        spotLight.position = glm::vec3(115.0F, 5.0F, 30.0F);
        spotLight.ambient = glm::vec3(0.0F);
        spotLight.diffuse = glm::vec3(0.0F);
        spotLight.specular = glm::vec3(0.0F);
        spotLight.constant = 1.0F;
        spotLight.linear = 0.009F;
        spotLight.quadratic = 0.0032F;
        spotLight.cutOff = glm::cos(glm::radians(12.5F));
        spotLight.outerCutOff = glm::cos(glm::radians(15.0F));

        transform = glm::translate(glm::mat4(1.0F), spotLight.position);
        transform = glm::scale(transform, glm::vec3(20.0F));

        const auto front = transform[2];
        spotLight.direction = -glm::vec3(front.x, front.y, front.z);
        // move position to the front of the light
        spotLight.position += spotLight.direction * 0.5F;
    }
};


#endif //SPOTLIGHT_H
