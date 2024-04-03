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

    // copy
    Renderable(const Renderable &other) = default;

    auto operator=(const Renderable &other) -> Renderable & = default;

    // move
    Renderable(Renderable &&other) noexcept = default;

    auto operator=(Renderable &&other) noexcept -> Renderable & = default;

    virtual void draw(std::shared_ptr<Shader> shader) const = 0;

    virtual void draw(const glm::mat4 &view, const glm::mat4 &projection) const;

    virtual void draw() const;

    void setShader(std::shared_ptr<Shader> shader);

    [[nodiscard]] auto getShader() const -> std::shared_ptr<Shader>;

protected:
    std::shared_ptr<Shader> shader;
};


#endif //CW_RENDERABLE_H
