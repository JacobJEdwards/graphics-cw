//
// Created by Jacob Edwards on 26/04/2024.
//

#ifndef TREES_H
#define TREES_H

#include <memory>
#include <vector>
#include "graphics/Model.h"
#include "renderables/Renderable.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <span>
#include <GL/glew.h>
#include <utils/ShaderManager.h>
#include "utils/BoundingBox.h"
#include "utils/Random.h"


// instanced trees
class Trees final : public Renderable {
public:
    using Renderable::draw;

    Trees();

    void generateTrees(std::span<const glm::vec3> positions);

    void draw(std::shared_ptr<Shader> shader) const override;

    auto getBoundingBoxes() const -> const std::vector<BoundingBox> &;

private
:
    std::vector<glm::vec3> trees;
    GLuint instanceVBO = 0;
    Model model;
    std::vector<BoundingBox> boundingBoxes;

    void setupInstanceData();
};


#endif //TREES_H
