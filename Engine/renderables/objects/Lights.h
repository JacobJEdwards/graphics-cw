//
// Created by Jacob Edwards on 12/05/2024.
//

#ifndef LIGHTOBJECTS_H
#define LIGHTOBJECTS_H

#include <App.h>
#include <graphics/Model.h>
#include <utils/ShaderManager.h>

#include "renderables/Renderable.h"
#include "utils/Lights.h"

class LightObjects final : public Renderable {
public:
    using Renderable::draw;

    LightObjects();

    void draw(std::shared_ptr<Shader> shader) const override;

    [[nodiscard]] auto getPointLights() const -> std::vector<PointLight>;

    void update(float deltaTime);

private:
    std::array<glm::mat4, 4> tranforms;
    std::vector<PointLight> pointLights;

    Model lightModel;

    void setupLights();
};


#endif //LIGHTS_H
