//
// Created by Jacob Edwards on 23/03/2024.
//

#ifndef CW_RENDERABLE_H
#define CW_RENDERABLE_H

#include <memory>
#include "utils/Shader.h"
#include <glm/glm.hpp>

class Renderable {
public:
    virtual void draw(std::shared_ptr<Shader> shader) const = 0;

    virtual void draw(const glm::mat4 &view, const glm::mat4 &projection) const = 0;

    virtual void update(float deltaTime) = 0;

    void setShader(std::shared_ptr<Shader> shader) {
        Renderable::shader = shader;
    }

    [[nodiscard]] auto getShader() const -> std::shared_ptr<Shader> {
        return shader;
    }

protected:
    std::shared_ptr<Shader> shader;
};


#endif //CW_RENDERABLE_H
