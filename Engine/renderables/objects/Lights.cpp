//
// Created by Jacob Edwards on 12/05/2024.
//

#include "Lights.h"

//
// Created by Jacob Edwards on 12/05/2024.
//

#include <App.h>
#include <graphics/Model.h>
#include <utils/ShaderManager.h>
#include <vector>

#include "renderables/Renderable.h"
#include "utils/Lights.h"

LightObjects::LightObjects() : lightModel("../Assets/objects/lights/bulb.obj") {
    shader = ShaderManager::GetInstance().get("Untextured");
    pointLights.reserve(16);

    setupLights();
}

void LightObjects::draw(const std::shared_ptr<Shader> shader) const {
    shader->use();
    for (int i = 0; i < 4; i++) {
        shader->setUniform("model", tranforms[i]);
        shader->setUniform("color", glm::vec3(1.0F));
        lightModel.draw(shader);
    }
}

[[nodiscard]] auto LightObjects::getPointLights() const -> std::vector<PointLight> {
    return pointLights;
}

void LightObjects::update(const float deltaTime) {
    const float time = App::view.getTime();
    // circle through colours based on time


    for (int i = 0; i < 4; i++) {
        const float red = std::sin(time * 2.0F + i) / 2.0F + 0.5F;
        const float green = std::sin(time * 2.0F + i + 2) / 2.0F + 0.5F;
        const float blue = std::sin(time * 2.0F + i + 4) / 2.0F + 0.5F;

        pointLights[i].ambient = glm::vec3(red, green, blue);
        pointLights[i].diffuse = glm::vec3(red, green, blue);
        pointLights[i].specular = glm::vec3(red, green, blue);
    }
}

void LightObjects::setupLights() {
    for (int i = 0; i < 4; i++) {
        // draw in a square
        PointLight pointLight;
        pointLight.position = glm::vec3(0.0F);
        pointLight.position.x = 103.0F * cos(glm::radians(90.0F * i));
        pointLight.position.z = 103.0F * sin(glm::radians(90.0F * i));
        pointLight.position.y = 7.8F;

        pointLight.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        pointLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        pointLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
        pointLight.constant = 1.0F;
        pointLight.linear = 0.09F;
        pointLight.quadratic = 0.05F;

        tranforms[i] = glm::mat4(1.0f);
        tranforms[i] = glm::translate(tranforms[i], pointLight.position);
        tranforms[i] = glm::rotate(tranforms[i], glm::radians(180.0F), glm::vec3(1.0F, 0.0F, 0.0F));

        tranforms[i] = glm::scale(tranforms[i], glm::vec3(10.0F));
        pointLight.position.y = 9.2F;
        pointLights.push_back(pointLight);
    }
}
