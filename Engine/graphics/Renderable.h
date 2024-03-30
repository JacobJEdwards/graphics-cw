//
// Created by Jacob Edwards on 23/03/2024.
//

#ifndef CW_RENDERABLE_H
#define CW_RENDERABLE_H

#include <memory>
#include "utils/Shader.h"
#include <glm/glm.hpp>
#include <utility>

class Renderable {
public:
    Renderable() = default;

    virtual ~Renderable() = default;

    // for shaders that don't require a view or projection matrix, only a model matrix
    virtual void draw(std::shared_ptr<Shader> shader) const = 0;

    // for use with associated shader
    virtual void draw(const glm::mat4 &view, const glm::mat4 &projection) const = 0;

    void setShader(std::shared_ptr<Shader> shader);

    [[nodiscard]] auto getShader() const -> std::shared_ptr<Shader>;

protected:
    std::shared_ptr<Shader> shader;
};


#endif //CW_RENDERABLE_H
