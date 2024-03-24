#ifndef SPLINE_H
#define SPLINE_H

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/spline.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <utility>
#include <vector>
#include <span>

namespace Physics {
    class Spline {
    public:
        enum class Type {
            CATMULLROM,
            HERMITE,
            CUBIC
        };

        explicit Spline(std::span<const glm::vec3> points, Type type = Type::CATMULLROM) : points(points.begin(),
                                                                                                  points.end()),
                                                                                           type(type),
                                                                                           numPoints(points.size()) {}

        [[nodiscard]] auto getPoint() const -> glm::vec3 {
            // float local_t = glm::fract(t);
            switch (type) {
                case Type::CATMULLROM:
                    return glm::catmullRom(points[p0Index], points[p1Index], points[p2Index],
                                           points[p3Index], t);
                case Type::HERMITE:
                    return glm::hermite(points[p0Index], points[p1Index], points[p2Index],
                                        points[p3Index], t);
                case Type::CUBIC:
                    return glm::cubic(points[p0Index], points[p1Index], points[p2Index],
                                      points[p3Index], t);
            }
        }

        void update() {
            /*
            p0Index = glm::clamp<int>(t - 1, 0, points.size() - 1);
            p1Index = glm::clamp<int>(t, 0, points.size() - 1);
            p2Index = glm::clamp<int>(t + 1, 0, points.size() - 1);
            p3Index = glm::clamp<int>(t + 2, 0, points.size() - 1);
            t += dt;
             */

            if (p3Index == numPoints) {
                p3Index = 0;
            }

            if (p2Index == numPoints) {
                p2Index = 0;
            }

            if (p1Index == numPoints) {
                p1Index = 0;
            }

            if (p0Index == numPoints) {
                p0Index = 0;
            }

            t += dt;

            if (t > 1.0F) {
                t = 0.0F;
                p0Index++;
                p1Index++;
                p2Index++;
                p3Index++;
            }

        }

    private:
        std::vector<glm::vec3> points;
        Type type;
        size_t numPoints;

        float dt = 0.5F;
        float t = 0.0F;

        int p0Index = 0;
        int p1Index = 1;
        int p2Index = 2;
        int p3Index = 3;
    };
} // namespace Physics

#endif
