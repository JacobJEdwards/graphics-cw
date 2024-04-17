#ifndef BUMPERCAR_H
#define BUMPERCAR_H

#include "graphics/Model.h"
#include "physics/Spline.h"
#include "graphics/Shader.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <vector>
#include "renderables/Entity.h"

class BumperCar final : public Entity {
public:
    using Entity::draw;

    enum class Mode {
        PATHED,
        TRACK,
        AUTO,
        NONE
    };

    explicit BumperCar(glm::vec2 centre = {0.0F, 0.0F}, float radius = 50.0F, float speed = 1.0F);

    explicit BumperCar(std::vector<glm::vec3> points, float speed = 1.0F);

    void update(float deltaTime) override;

    void draw(std::shared_ptr<Shader> shader) const override;

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    void setMode(Mode mode);

    void setSpeed(float speed);

    void invert();

    void moveTo(glm::vec3 position);

    void shouldDrawPlayer(bool draw);

    void addTrackablePosition(const glm::vec3 position) {
        trackablePositions.push_back(position);
    }

    void clearTrackablePositions() {
        trackablePositions.clear();
    }

    static void Interface();

private:
    Mode mode = Mode::AUTO;

    float speed = 1.0F;

    Physics::Spline spline;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> trackablePositions;

    std::unique_ptr<Model> person;

    static float coneRadius;
    static float coneHeight;
    static bool paused;
    static float trackingDistance;
    static float ventureDistance;

    bool drawPlayer = true;
};

#endif
