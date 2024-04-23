//
// Created by Jacob Edwards on 17/04/2024.
//

#include "Plane.h"
#include "graphics/buffers/VertexBuffer.h"

Plane::Plane() {
    buffer = std::make_unique<VertexBuffer>();
    buffer->fill(vertices);
}

void Plane::draw() const {
    buffer->bind();
    buffer->draw();
    buffer->unbind();
}

