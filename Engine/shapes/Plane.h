#ifndef PLANE_H
#define PLANE_H

#include "utils/Buffer.h"
#include "utils/Vertex.h"
#include <memory>
#include <vector>

class Plane {
public:
    Plane() {
        buffer = std::make_unique<Buffer>();
        buffer->fill(vertices);
    }

    void draw() const {
        buffer->bind();
        buffer->draw();
        buffer->unbind();
    }

private:
    std::unique_ptr<Buffer> buffer;

    std::vector<Vertex::Data> vertices{
            Vertex::Data{glm::vec3(-1.0F, 1.0F, 0.0F), glm::vec2(0.0F, 1.0F)},
            Vertex::Data{glm::vec3(-1.0F, -1.0F, 0.0F), glm::vec2(0.0F, 0.0F)},
            Vertex::Data{glm::vec3(1.0F, -1.0F, 0.0F), glm::vec2(1.0F, 0.0F)},
            Vertex::Data{glm::vec3(-1.0F, 1.0F, 0.0F), glm::vec2(0.0F, 1.0F)},
            Vertex::Data{glm::vec3(1.0F, -1.0F, 0.0F), glm::vec2(1.0F, 0.0F)},
            Vertex::Data{glm::vec3(1.0F, 1.0F, 0.0F), glm::vec2(1.0F, 1.0F)}};
};

#endif
