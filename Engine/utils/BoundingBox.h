//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <assimp/vector3.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include <utility>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <renderables/Renderable.h>

#include "graphics/buffers/VertexBuffer.h"

class BoundingBox final : public Renderable {
public:
    using Renderable::draw;

    BoundingBox();

    BoundingBox(glm::vec3 min, glm::vec3 max);

    BoundingBox(const aiVector3D &min, const aiVector3D &max);

    // destructor
    ~BoundingBox() override = default;

    // copy constructor
    BoundingBox(const BoundingBox &other);

    // copy assignment
    auto operator=(const BoundingBox &other) -> BoundingBox &;

    // move constructor
    BoundingBox(BoundingBox &&other) noexcept;

    // move assignment
    auto operator=(BoundingBox &&other) noexcept -> BoundingBox &;

    [[nodiscard]] auto getMin() const -> glm::vec3;

    [[nodiscard]] auto getMax() const -> glm::vec3;

    [[nodiscard]] auto getMinMax() const -> std::pair<glm::vec3, glm::vec3>;

    void setMin(const glm::vec3 &min);

    void setMax(const glm::vec3 &max);

    void transform(const glm::mat4 &model);

    [[nodiscard]] auto collides(const BoundingBox &other) const -> bool;

    [[nodiscard]] auto contains(const glm::vec3 &point) const -> bool;

    [[nodiscard]] auto contains(const BoundingBox &other) const -> bool;

    [[nodiscard]] auto isColliding(const BoundingBox &other) const -> bool;

    void setPosition(const glm::vec3 &position);

    [[nodiscard]] auto getCenter() const -> glm::vec3;

    [[nodiscard]] auto getSize() const -> glm::vec3;

    void translate(const glm::vec3 &translation);

    void scale(const glm::vec3 &scale);

    void rotate(const glm::vec3 &axis, float angle);

    void rotate(const glm::vec3 &rotation);

    [[nodiscard]] auto getCorners() const -> std::vector<glm::vec3>;

    [[nodiscard]] auto getOffset(const glm::vec3 &point) const -> glm::vec3;

    [[nodiscard]] auto getOffset(const BoundingBox &other) const -> glm::vec3;

    void expand(const glm::vec3 &amount);

    void expand(const BoundingBox &other);

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    void draw() const override;

    [[nodiscard]] auto getCollisionPoint(const BoundingBox &box) const -> glm::vec3;

    void addChildren(std::vector<std::unique_ptr<BoundingBox> > &children);

    void addChild(std::unique_ptr<BoundingBox> &child);

    void addChild(const BoundingBox &box);

    void setParent(BoundingBox *parent);

    [[nodiscard]] auto getChildren() const -> const std::vector<std::unique_ptr<BoundingBox> > &;

    [[nodiscard]] auto getParent() const -> BoundingBox *;

private:
    glm::vec3 min{};
    glm::vec3 max{};

    std::unique_ptr<VertexBuffer> buffer;

    std::vector<std::unique_ptr<BoundingBox> > children;
    BoundingBox *parent = nullptr;

    void initBuffer();
};

#endif // BOUNDINGBOX_H
