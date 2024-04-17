//
// Created by Jacob Edwards on 17/04/2024.
//

#include "Plane.h"
#include "graphics/buffers/Buffer.h"

Plane::Plane() {
    buffer = std::make_unique<Buffer>();
    buffer->fill(vertices);
}

void Plane::draw() const {
    buffer->bind();
    buffer->draw();
    buffer->unbind();
}

