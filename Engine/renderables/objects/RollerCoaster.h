//
// Created by Jacob Edwards on 28/04/2024.
//

#ifndef ROLLERCOASTER_H
#define ROLLERCOASTER_H

#include "graphics/Model.h"
#include "graphics/Shader.h"
#include "utils/ShaderManager.h"
#include "renderables/Renderable.h"
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class RollerCoaster final : public Renderable {
public:
    using Renderable::draw;

    RollerCoaster();

    void draw(std::shared_ptr<Shader> shader) const override;

private:
    Model model;
    glm::vec3 position = glm::vec3(-400.0F, 0.0F, 50.0F);
    glm::vec3 scale = glm::vec3(4.0F);
    glm::vec3 rotation = glm::vec3(0.0F);
    glm::mat4 modelMatrix = glm::mat4(1.0F);
};


#endif //ROLLERCOASTER_H
