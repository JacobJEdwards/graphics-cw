#ifndef SPLINE_H
#define SPLINE_H

#include <glm/glm.hpp>

#include <utility>
#include <vector>
#include <span>

namespace Physics {
    class Spline {
    public:
        enum class Type {
            CATMULLROM,
            HERMITE,
            CUBIC,
            BEZIER,
            LINEAR
        };

        explicit Spline(std::span<const glm::vec3> points, Type type = Type::CATMULLROM);

        [[nodiscard]] auto getPoint() const -> glm::vec3;

        void update(float dt);

    private:
        std::vector<glm::vec3> points;
        Type type;
        std::size_t numPoints;

        std::size_t p0Index = 0;
        std::size_t p1Index = 1;
        std::size_t p2Index = 2;
        std::size_t p3Index = 3;

        float t = 0.0F;
    };
} // namespace Physics

#endif
