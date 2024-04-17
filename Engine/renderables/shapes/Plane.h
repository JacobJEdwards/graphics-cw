#ifndef PLANE_H
#define PLANE_H

#include "graphics/buffers/Buffer.h"
#include "graphics/Vertex.h"
#include <memory>
#include <vector>
#include "renderables//Renderable.h"

class Plane final : public Renderable {
public:
    Plane();

    void draw() const override;

private:
    std::unique_ptr<Buffer> buffer;

    std::vector<Vertex::Data> vertices{
        Vertex::Data{glm::vec3(-1.0F, 1.0F, 0.0F), glm::vec2(0.0F, 1.0F)},
        Vertex::Data{glm::vec3(-1.0F, -1.0F, 0.0F), glm::vec2(0.0F, 0.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, 0.0F), glm::vec2(1.0F, 0.0F)},
        Vertex::Data{glm::vec3(-1.0F, 1.0F, 0.0F), glm::vec2(0.0F, 1.0F)},
        Vertex::Data{glm::vec3(1.0F, -1.0F, 0.0F), glm::vec2(1.0F, 0.0F)},
        Vertex::Data{glm::vec3(1.0F, 1.0F, 0.0F), glm::vec2(1.0F, 1.0F)}
    };
};

#endif
