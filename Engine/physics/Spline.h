#ifndef SPLINE_H
#define SPLINE_H

#include <cstddef>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

#include <utility>
#include <vector>
#include <span>
#include <memory>
#include "utils/Buffer.h"
#include "utils/Shader.h"
#include "graphics/Renderable.h"

namespace Physics {
    class Spline : public Renderable {
    public:
        enum class Type {
            CATMULLROM,
            HERMITE,
            CUBIC,
            BEZIER,
            LINEAR
        };

        Spline() = default;

        explicit Spline(std::span<const glm::vec3> points, Type type = Type::CATMULLROM, float speed = 1.0F);

        [[nodiscard]] auto getPoints() const -> const std::vector<glm::vec3> &;

        [[nodiscard]] auto getPoint() const -> glm::vec3;

        void update(float dt);

        void draw(std::shared_ptr<Shader> shader) const override;

        void setSpeed(float speed);

        void invert();

    private:
        std::vector<glm::vec3> points;
        Type type;
        float speed;
        std::size_t numPoints;

        std::size_t p0Index = 0;
        std::size_t p1Index = 1;
        std::size_t p2Index = 2;
        std::size_t p3Index = 3;

        std::unique_ptr<Buffer> buffer;

        float t = 0.0F;
    };
} // namespace Physics

#endif
