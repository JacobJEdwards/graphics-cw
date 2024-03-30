//
// Created by Jacob Edwards on 29/03/2024.
//

#ifndef CW_IMMOVABLEOBJECT_H
#define CW_IMMOVABLEOBJECT_H

#include "graphics/Renderable.h"
#include "physics/ModelAttributes.h"
#include "graphics/Model.h"
#include "utils/BoundingBox.h"

class ImmovableObject : public Renderable {
public:
    ImmovableObject() = default;

    ~ImmovableObject() override = default;

    ImmovableObject(const ImmovableObject &other) = delete;

    auto operator=(const ImmovableObject &other) -> ImmovableObject & = delete;

    ImmovableObject(ImmovableObject &&other) noexcept = default;

    auto operator=(ImmovableObject &&other) noexcept -> ImmovableObject & = default;

    [[nodiscard]] auto getBoundingBox() -> BoundingBox &;

    [[nodiscard]] auto getAttributes() -> Physics::Attributes &;

    [[nodiscard]] auto getNormal() const -> glm::vec3;

protected:
    Physics::Attributes attributes;

    BoundingBox box;
    glm::vec3 normal;
};


#endif //CW_IMMOVABLEOBJECT_H
