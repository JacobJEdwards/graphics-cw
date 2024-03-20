//
// Created by Jacob Edwards on 13/03/2024.
//

#include "BoundingBox.h"

#include "helpers/AssimpGLMHelpers.h"
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "utils/Vertex.h"
#include "utils/ShaderManager.h"


BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max) {
    initBuffer();
}

BoundingBox::BoundingBox(const aiVector3D &min, const aiVector3D &max)
        : min(AssimpGLMHelpers::getGLMVec(min)),
          max(AssimpGLMHelpers::getGLMVec(max)) {
    initBuffer();
}

[[nodiscard]] auto BoundingBox::getMin() const -> glm::vec3 { return min; }

[[nodiscard]] auto BoundingBox::getMax() const -> glm::vec3 { return max; }

void BoundingBox::setMin(const glm::vec3 &min) { BoundingBox::min = min; }

void BoundingBox::setMax(const glm::vec3 &max) { BoundingBox::max = max; }

void BoundingBox::transform(const glm::mat4 &model) {
    glm::vec3 newMin = glm::vec3(model * glm::vec4(min, 1.0F));
    glm::vec3 newMax = glm::vec3(model * glm::vec4(max, 1.0F));

    min = glm::min(newMin, newMax);
    max = glm::max(newMin, newMax);
}

auto BoundingBox::collides(const BoundingBox &other) const -> bool {
    for (int i = 0; i < 3; ++i) {
        if (max[i] < other.min[i] || min[i] > other.max[i]) {
            // No overlap on this axis, so no collision
            return false;
        }
    }
    return true;
}

auto BoundingBox::contains(const glm::vec3 &point) const -> bool {
    return point.x >= min.x && point.x <= max.x && point.y >= min.y &&
           point.y <= max.y && point.z >= min.z && point.z <= max.z;
}

auto BoundingBox::contains(const BoundingBox &other) const -> bool {
    return contains(other.min) && contains(other.max);
}

auto BoundingBox::isColliding(const BoundingBox &other) const -> bool {
    return collides(other) || contains(other) || other.contains(*this);
}

void BoundingBox::setPosition(const glm::vec3 &position) {
    auto center = getCenter();
    auto offset = position - center;

    translate(offset);
}

auto BoundingBox::getCenter() const -> glm::vec3 { return (min + max) / 2.0F; }

auto BoundingBox::getSize() const -> glm::vec3 { return max - min; }

void BoundingBox::translate(const glm::vec3 &translation) {
    min += translation;
    max += translation;
}

void BoundingBox::scale(const glm::vec3 &scale) {
    min *= scale;
    max *= scale;
}

void BoundingBox::rotate(const glm::vec3 &axis, float angle) {
    auto center = getCenter();
    auto rotationMatrix = glm::rotate(glm::mat4(1.0F), angle, axis);
    min = glm::vec3(rotationMatrix * glm::vec4(min - center, 1.0F)) + center;
    max = glm::vec3(rotationMatrix * glm::vec4(max - center, 1.0F)) + center;
}

auto BoundingBox::getOffset(const glm::vec3 &point) const -> glm::vec3 {
    if (!contains(point)) {
        return glm::vec3(0.0F);
    }

    float distanceToXMin = std::abs(point.x - min.x);
    float distanceToXMax = std::abs(point.x - max.x);
    float distanceToYMin = std::abs(point.y - min.y);
    float distanceToYMax = std::abs(point.y - max.y);
    float distanceToZMin = std::abs(point.z - min.z);
    float distanceToZMax = std::abs(point.z - max.z);

    if (distanceToXMin < distanceToXMax && distanceToXMin < distanceToYMin &&
        distanceToXMin < distanceToYMax && distanceToXMin < distanceToZMin &&
        distanceToXMin < distanceToZMax) {
        return {-distanceToXMin, 0.0F, 0.0F};
    }

    if (distanceToXMax < distanceToXMin && distanceToXMax < distanceToYMin &&
        distanceToXMax < distanceToYMax && distanceToXMax < distanceToZMin &&
        distanceToXMax < distanceToZMax) {
        return {distanceToXMax, 0.0F, 0.0F};
    }

    if (distanceToYMin < distanceToXMin && distanceToYMin < distanceToXMax &&
        distanceToYMin < distanceToYMax && distanceToYMin < distanceToZMin &&
        distanceToYMin < distanceToZMax) {
        return {0.0F, -distanceToYMin, 0.0F};
    }

    if (distanceToYMax < distanceToXMin && distanceToYMax < distanceToXMax &&
        distanceToYMax < distanceToYMin && distanceToYMax < distanceToZMin &&
        distanceToYMax < distanceToZMax) {
        return {0.0F, distanceToYMax, 0.0F};
    }

    if (distanceToZMin < distanceToXMin && distanceToZMin < distanceToXMax &&
        distanceToZMin < distanceToYMin && distanceToZMin < distanceToYMax &&
        distanceToZMin < distanceToZMax) {
        return {0.0F, 0.0F, -distanceToZMin};
    }

    if (distanceToZMax < distanceToXMin && distanceToZMax < distanceToXMax &&
        distanceToZMax < distanceToYMin && distanceToZMax < distanceToYMax &&
        distanceToZMax < distanceToZMin) {
        return {0.0F, 0.0F, distanceToZMax};
    }

    return glm::vec3(0.0F);
}

auto BoundingBox::getOffset(const BoundingBox &other) const -> glm::vec3 {
    glm::vec3 overlap = glm::min(max, other.max) - glm::max(min, other.min);

    if (overlap.x < 0 || overlap.y < 0 || overlap.z < 0) {
        return glm::vec3(0.0F);
    }

    float minOverlap = std::min(std::min(overlap.x, overlap.y), overlap.z);

    if (overlap.x == minOverlap) {
        return {overlap.x, 0.0F, 0.0F};
    }
    if (overlap.y == minOverlap) {
        return glm::vec3(0.0f, overlap.y, 0.0f);
    } else {
        return glm::vec3(0.0f, 0.0f, overlap.z);
    }
}

void BoundingBox::expand(const glm::vec3 &amount) {
    min -= amount;
    max += amount;
}

void BoundingBox::expand(const BoundingBox &other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
}

void BoundingBox::initBuffer() {
    buffer.drawMode = GL_LINES;

    std::vector<Vertex::Data> vertices = {
            {{min.x, min.y, min.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{max.x, min.y, min.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{max.x, max.y, min.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{min.x, max.y, min.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{min.x, min.y, max.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{max.x, min.y, max.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{max.x, max.y, max.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{min.x, max.y, max.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
    };

    std::vector<GLuint> indices = {
            0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 5, 4, 7, 7, 6, 5,
            4, 0, 3, 3, 7, 4, 3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4,
    };

    buffer.fill(vertices, indices);
}


void BoundingBox::draw(glm::mat4 model, glm::mat4 view,
                       glm::mat4 projection) const {
    ShaderManager::Get("BoundingBox")->use();
    ShaderManager::Get("BoundingBox")->setUniform("model", model);
    ShaderManager::Get("BoundingBox")->setUniform("view", view);
    ShaderManager::Get("BoundingBox")->setUniform("projection", projection);

    buffer.bind();
    buffer.draw();
    buffer.unbind();
}

auto BoundingBox::getCollisionPoint(const BoundingBox &box) const -> glm::vec3 {
    glm::vec3 overlap = glm::min(max, box.max) - glm::max(min, box.min);

    if (overlap.x < 0 || overlap.y < 0 || overlap.z < 0) {
        return glm::vec3(0.0F);
    }

    float minOverlap = std::min(std::min(overlap.x, overlap.y), overlap.z);

    if (overlap.x == minOverlap) {
        return {overlap.x, 0.0F, 0.0F};
    }

    if (overlap.y == minOverlap) {
        return {0.0F, overlap.y, 0.0F};
    }

    return {0.0F, 0.0F, overlap.z};

}
