//
// Created by Jacob Edwards on 11/05/2024.
//

#ifndef BARRIERS_H
#define BARRIERS_H
#include "graphics/Shader.h"
#include <renderables/Entity.h>
#include <utils/ShaderManager.h>


class Barriers final : public Renderable {
public:
    using Renderable::draw;

    Barriers();

    void draw(std::shared_ptr<Shader> shader) const override;

private:
    std::vector<glm::mat4> transforms;
    Model model;
};


#endif //BARRIERS_H
