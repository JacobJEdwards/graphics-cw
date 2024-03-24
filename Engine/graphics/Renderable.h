//
// Created by Jacob Edwards on 23/03/2024.
//

#ifndef CW_RENDERABLE_H
#define CW_RENDERABLE_H

#include <memory>
#include "utils/Shader.h"
#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"

class Renderable {
public:
    virtual void draw() const = 0;

    virtual void update(float dt) = 0;

    virtual void setShader(const std::shared_ptr<Shader> &shader) = 0;

    virtual ~Renderable() = default;

    Renderable(const Renderable &other) = delete;

    virtual auto operator=(const Renderable &other) -> Renderable & = delete;

    [[nodiscard]] auto getAttributes() -> Physics::Attributes & { return attributes; }

    [[nodiscard]] auto getBoundingBox() const -> BoundingBox { return box; }


protected:
    std::shared_ptr<Shader> shader;
    Physics::Attributes attributes;
    BoundingBox box;

    explicit Renderable(std::shared_ptr<Shader> shader) : shader(std::move(shader)) {}
};


#endif //CW_RENDERABLE_H
