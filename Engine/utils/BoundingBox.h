//
// Created by Jacob Edwards on 13/03/2024.
//

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "helpers/AssimpGLMHelpers.h"
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class BoundingBox {
public:
    BoundingBox(glm::vec3 min, glm::vec3 max)
        : min(min)
        , max(max)
    {
    }
    BoundingBox(const aiVector3D& min, const aiVector3D& max)
        : min(AssimpGLMHelpers::getGLMVec(min))
        , max(AssimpGLMHelpers::getGLMVec(max))
    {
    }

    [[nodiscard]] auto getMin() const -> glm::vec3
    {
        return min;
    }

    [[nodiscard]] auto getMax() const -> glm::vec3
    {
        return max;
    }

    void setMin(const glm::vec3& min)
    {
        BoundingBox::min = min;
    }

    void setMax(const glm::vec3& max)
    {
        BoundingBox::max = max;
    }

    bool detectCollision(const BoundingBox& other) const
    {
        return (min.x <= other.getMax().x && max.x >= other.getMin().x) && (min.y <= other.getMax().y && max.y >= other.getMin().y) && (min.z <= other.getMax().z && max.z >= other.getMin().z);
    }

    bool contains(const glm::vec3& point) const
    {
        return point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y && point.z >= min.z && point.z <= max.z;
    }

    glm::vec3 getCenter() const
    {
        return (min + max) / 2.0F;
    }

    glm::vec3 getSize() const
    {
        return max - min;
    }

    void translate(const glm::vec3& translation)
    {
        min += translation;
        max += translation;
    }

    void scale(const glm::vec3& scale)
    {
        min *= scale;
        max *= scale;
    }

    glm::vec3 getOffset(const glm::vec3& point) const
    {
        if (!contains(point)) {
            return glm::vec3(0.0F);
        }

        float distanceToXMin = std::abs(point.x - min.x);
        float distanceToXMax = std::abs(point.x - max.x);
        float distanceToYMin = std::abs(point.y - min.y);
        float distanceToYMax = std::abs(point.y - max.y);
        float distanceToZMin = std::abs(point.z - min.z);
        float distanceToZMax = std::abs(point.z - max.z);

        if (distanceToXMin < distanceToXMax && distanceToXMin < distanceToYMin && distanceToXMin < distanceToYMax && distanceToXMin < distanceToZMin && distanceToXMin < distanceToZMax) {
            return glm::vec3(-distanceToXMin, 0.0F, 0.0F);
        }

        if (distanceToXMax < distanceToXMin && distanceToXMax < distanceToYMin && distanceToXMax < distanceToYMax && distanceToXMax < distanceToZMin && distanceToXMax < distanceToZMax) {
            return glm::vec3(distanceToXMax, 0.0F, 0.0F);
        }

        if (distanceToYMin < distanceToXMin && distanceToYMin < distanceToXMax && distanceToYMin < distanceToYMax && distanceToYMin < distanceToZMin && distanceToYMin < distanceToZMax) {
            return glm::vec3(0.0F, -distanceToYMin, 0.0F);
        }

        if (distanceToYMax < distanceToXMin && distanceToYMax < distanceToXMax && distanceToYMax < distanceToYMin && distanceToYMax < distanceToZMin && distanceToYMax < distanceToZMax) {
            return glm::vec3(0.0F, distanceToYMax, 0.0F);
        }

        if (distanceToZMin < distanceToXMin && distanceToZMin < distanceToXMax && distanceToZMin < distanceToYMin && distanceToZMin < distanceToYMax && distanceToZMin < distanceToZMax) {
            return glm::vec3(0.0F, 0.0F, -distanceToZMin);
        }

        if (distanceToZMax < distanceToXMin && distanceToZMax < distanceToXMax && distanceToZMax < distanceToYMin && distanceToZMax < distanceToYMax && distanceToZMax < distanceToZMin) {
            return glm::vec3(0.0F, 0.0F, distanceToZMax);
        }

        return glm::vec3(0.0F);
    }

private:
    glm::vec3 min;
    glm::vec3 max;
};

#endif // BOUNDINGBOX_H
