//
// Created by Jacob Edwards on 28/04/2024.
//

#ifndef FERRISWHEEL_H
#define FERRISWHEEL_H


#include "Config.h"
#include "graphics/Shader.h"
#include <utils/ShaderManager.h>

#include "renderables/Renderable.h"
#include "graphics/Model.h"
#include <memory>
#include <glm/ext/matrix_transform.hpp>

class FerrisWheel final : public Renderable {
public:
    using Renderable::draw;

    FerrisWheel();

    void draw(std::shared_ptr<Shader> shader) const override;

    void update(float deltaTime);

private:
    Model staticPart;
    Model rotatingPart;
    Model cabin;

    Model entrance;

    glm::mat4 staticPartTransform = Config::IDENTITY_MATRIX;
    glm::mat4 rotatingPartTransform = Config::IDENTITY_MATRIX;
    glm::mat4 cabinTransform = Config::IDENTITY_MATRIX;

    glm::mat4 entranceTransform = Config::IDENTITY_MATRIX;

    glm::vec3 scale = glm::vec3(10.0F);
    glm::vec3 translation = glm::vec3(20.0F, 0.0F, 0.0F);

    float radius = 4.45F * scale.y;
    glm::vec3 centre = glm::vec3(0.0F, radius, 0.0F);
};


#endif //FERRISWHEEL_H
