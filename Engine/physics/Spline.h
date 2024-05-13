#ifndef SPLINE_H
#define SPLINE_H

/*
 * https://en.wikipedia.org/wiki/Cubic_Hermite_spline
 */

#include <cstddef>
#include <glm/ext/vector_float3.hpp>

#include <vector>
#include <span>
#include <memory>
#include "graphics/buffers/VertexBuffer.h"
#include "graphics/Shader.h"
#include "renderables/Renderable.h"

namespace Physics {
    class Spline final : public Renderable {
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

        [[nodiscard]] auto getLaps() const -> std::size_t;

        void invert();

        void randomise();

    private:
        std::vector<glm::vec3> points;
        Type type = Type::CATMULLROM;
        float speed{};
        std::size_t numPoints{};

        std::size_t p0Index = 0U;
        std::size_t p1Index = 1U;
        std::size_t p2Index = 2U;
        std::size_t p3Index = 3U;

        std::size_t laps = 0U;

        std::unique_ptr<VertexBuffer> buffer;

        float t = 0.0F;
    };
} // namespace Physics

#endif
