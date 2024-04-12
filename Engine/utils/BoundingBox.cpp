//
// Created by Jacob Edwards on 13/03/2024.
//

#include "BoundingBox.h"

#include "Config.h"
#include "Buffer.h"
#include "Shader.h"
#include "helpers/AssimpGLMHelpers.h"
#include <algorithm>
#include <assimp/vector3.h>
#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/common.hpp>
#include <glm/ext/vector_float3.hpp>

#include <utility>
#include <vector>
#include <GL/glew.h>
#include <memory>

#include "utils/Vertex.h"
#include "utils/ShaderManager.h"
#include "App.h"


BoundingBox::BoundingBox(const glm::vec3 min, const glm::vec3 max) : min(min), max(max) {
    initBuffer();
}

BoundingBox::BoundingBox(const aiVector3D &min, const aiVector3D &max)
        : min(AssimpGLMHelpers::getGLMVec(min)),
          max(AssimpGLMHelpers::getGLMVec(max)) {
    initBuffer();
}

BoundingBox::BoundingBox(const BoundingBox &other) {
    min = other.min;
    max = other.max;
    buffer = std::make_unique<Buffer>(*other.buffer);
    children.clear();
    for (const auto &child: other.children) {
        children.push_back(std::make_unique<BoundingBox>(*child));
    }
    if (other.parent != nullptr) {
        parent = other.parent;
    }
}

// copy assignment
auto BoundingBox::operator=(const BoundingBox &other) -> BoundingBox & {
    if (this != &other) {
        min = other.min;
        max = other.max;
        buffer = std::make_unique<Buffer>(*other.buffer);

        children.clear();
        for (const auto &child: other.children) {
            children.push_back(std::make_unique<BoundingBox>(*child));
        }
        if (other.parent != nullptr) {
            parent = other.parent;
        }

    }
    return *this;
}

BoundingBox::BoundingBox(BoundingBox &&other) noexcept {
    min = other.min;
    max = other.max;
    buffer = std::move(other.buffer);
    children = std::move(other.children);
    parent = other.parent;
}

auto BoundingBox::operator=(BoundingBox &&other) noexcept -> BoundingBox & {
    if (this != &other) {
        min = other.min;
        max = other.max;
        buffer = std::move(other.buffer);
        children = std::move(other.children);
        parent = other.parent;
    }
    return *this;
}


[[nodiscard]] auto BoundingBox::getMin() const -> glm::vec3 {
    return glm::min(min, max);
}

[[nodiscard]] auto BoundingBox::getMax() const -> glm::vec3 {
    return glm::max(min, max);
}

void BoundingBox::setMin(const glm::vec3 &min) { BoundingBox::min = min; }

void BoundingBox::setMax(const glm::vec3 &max) { BoundingBox::max = max; }

void BoundingBox::transform(const glm::mat4 &model) {
    // Transform the min and max points by the model
    min = glm::vec3(model * glm::vec4(min, 1.0F));
    max = glm::vec3(model * glm::vec4(max, 1.0F));

    for (const auto &child: children) {
        child->transform(model);
    }
}

auto BoundingBox::collides(const BoundingBox &other) const -> bool {
    for (int i = 0; i < 3; ++i) {
        if (max[i] < other.min[i] || min[i] > other.max[i]) {
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
    const auto center = getCenter();
    const auto offset = position - center;

    translate(offset);
}

auto BoundingBox::getCenter() const -> glm::vec3 { return (min + max) / 2.0F; }

auto BoundingBox::getSize() const -> glm::vec3 { return max - min; }

void BoundingBox::translate(const glm::vec3 &translation) {
    min += translation;
    max += translation;

    for (const auto &child: children) {
        child->translate(translation);
    }
}

void BoundingBox::scale(const glm::vec3 &scale) {
    min *= scale;
    max *= scale;

    for (const auto &child: children) {
        child->scale(scale);
    }
}

void BoundingBox::rotate(const glm::vec3 &rotation) {
    const auto center = getCenter();
    auto rotationMatrix = glm::rotate(glm::mat4(1.0F), rotation.x, glm::vec3(1.0F, 0.0F, 0.0F));
    rotationMatrix = glm::rotate(rotationMatrix, rotation.y, glm::vec3(0.0F, 1.0F, 0.0F));
    rotationMatrix = glm::rotate(rotationMatrix, rotation.z, glm::vec3(0.0F, 0.0F, 1.0F));
    min = glm::vec3(rotationMatrix * glm::vec4(min - center, 1.0F)) + center;
    max = glm::vec3(rotationMatrix * glm::vec4(max - center, 1.0F)) + center;

    for (const auto &child: children) {
        child->rotate(rotation);
    }
}

void BoundingBox::rotate(const glm::vec3 &axis, const float angle) {
    const auto center = getCenter();
    const auto rotationMatrix = glm::rotate(glm::mat4(1.0F), angle, axis);
    min = glm::vec3(rotationMatrix * glm::vec4(min - center, 1.0F)) + center;
    max = glm::vec3(rotationMatrix * glm::vec4(max - center, 1.0F)) + center;

    for (const auto &child: children) {
        child->rotate(axis, angle);
    }
}

auto BoundingBox::getOffset(const glm::vec3 &point) const -> glm::vec3 {
    if (!contains(point)) {
        return glm::vec3(0.0F);
    }

    const float distanceToXMin = std::abs(point.x - min.x);
    const float distanceToXMax = std::abs(point.x - max.x);
    const float distanceToYMin = std::abs(point.y - min.y);
    const float distanceToYMax = std::abs(point.y - max.y);
    const float distanceToZMin = std::abs(point.z - min.z);
    const float distanceToZMax = std::abs(point.z - max.z);

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
    const glm::vec3 overlap = glm::min(max, other.max) - glm::max(min, other.min);

    if (overlap.x < 0 || overlap.y < 0 || overlap.z < 0) {
        return glm::vec3(0.0F);
    }

    const float minOverlap = std::min({overlap.x, overlap.y, overlap.z});

    if (overlap.x == minOverlap) {
        return {overlap.x, 0.0F, 0.0F};
    }
    if (overlap.y == minOverlap) {
        return {0.0F, overlap.y, 0.0F};
    }

    return {0.0F, 0.0F, overlap.z};

}

void BoundingBox::expand(const glm::vec3 &amount) {
    min -= amount;
    max += amount;

    for (const auto &child: children) {
        child->expand(amount);
    }
}

void BoundingBox::expand(const BoundingBox &other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);

    for (const auto &child: children) {
        child->expand(other);
    }
}

void BoundingBox::initBuffer() {
    buffer = std::make_unique<Buffer>();
    buffer->drawMode = GL_LINES;

    const std::vector<Vertex::Data> vertices = {
            {{min.x, min.y, min.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{max.x, min.y, min.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{max.x, max.y, min.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{min.x, max.y, min.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{min.x, min.y, max.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{max.x, min.y, max.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{max.x, max.y, max.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
            {{min.x, max.y, max.z}, {0.0F, 0.0F, 0.0F}, {0.0F, 0.0F}},
    };

    const std::vector<GLuint> indices = {
            0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 5, 4, 7, 7, 6, 5,
            4, 0, 3, 3, 7, 4, 3, 2, 6, 6, 7, 3, 4, 5, 1, 1, 0, 4,
    };

    buffer->fill(vertices, indices);
}

void BoundingBox::draw(const glm::mat4 &model, const glm::mat4 &view,
                       const glm::mat4 &projection) const {

    const GLuint previousProgram = ShaderManager::GetActiveShader();

    const std::shared_ptr<Shader> shader = ShaderManager::Get("BoundingBox");

    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);

    if (App::debug) {
        shader->setUniform("model", Config::IDENTITY_MATRIX);
        const auto buffer = std::make_unique<Buffer>();
        buffer->drawMode = GL_LINES;

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

        buffer->fill(vertices, indices);

        buffer->bind();
        buffer->draw();
        buffer->unbind();

        for (const auto &child: children) {
            child->draw(model, view, projection);
        }

    }

    glUseProgram(previousProgram);
}

void BoundingBox::draw() const {
    buffer->bind();
    buffer->draw();
    buffer->unbind();
}

auto BoundingBox::getCollisionPoint(const BoundingBox &box) const -> glm::vec3 {
    const glm::vec3 overlap = glm::min(max, box.max) - glm::max(min, box.min);

    if (overlap.x < 0.0F || overlap.y < 0.0F || overlap.z < 0.0F) {
        return glm::vec3(0.0F);
    }

    const float minOverlap = std::min(std::min(overlap.x, overlap.y), overlap.z);

    if (overlap.x == minOverlap) {
        return {overlap.x, 0.0F, 0.0F};
    }

    if (overlap.y == minOverlap) {
        return {0.0F, overlap.y, 0.0F};
    }

    return {0.0F, 0.0F, overlap.z};

}

auto BoundingBox::getMinMax() const -> std::pair<glm::vec3, glm::vec3> {
    return {
            glm::min(min, max),
            glm::max(min, max)
    };
}

void BoundingBox::addChildren(std::vector<std::unique_ptr<BoundingBox>> &children) {
    for (auto &child: children) {
        addChild(child);
    }
}

void BoundingBox::addChild(std::unique_ptr<BoundingBox> &child) {
    children.push_back(std::move(child));
    children.back()->setParent(this);
}

void BoundingBox::addChild(const BoundingBox &box) {
    children.push_back(std::make_unique<BoundingBox>(box));
    children.back()->setParent(this);
}

[[nodiscard]] auto BoundingBox::getChildren() const -> const std::vector<std::unique_ptr<BoundingBox>> & {
    return children;
}

[[nodiscard]] auto BoundingBox::getParent() const -> BoundingBox * {
    return parent;
}

void BoundingBox::setParent(BoundingBox *parent) {
    BoundingBox::parent = parent;
}

