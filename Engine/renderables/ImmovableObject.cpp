//
// Created by Jacob Edwards on 29/03/2024.
//

#include "renderables/ImmovableObject.h"

#include "utils/BoundingBox.h"
#include "physics/ModelAttributes.h"
#include <glm/ext/vector_float3.hpp>


[[nodiscard]] auto ImmovableObject::getBoundingBox() -> BoundingBox & {
    return box;
}

[[nodiscard]] auto ImmovableObject::getAttributes() -> Physics::Attributes & {
    return attributes;
}

[[nodiscard]] auto ImmovableObject::getNormal() const -> glm::vec3 {
    return normal;
}


