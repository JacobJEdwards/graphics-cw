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

    void reset();

    void draw(std::shared_ptr<Shader> shader) const override;

    void setMode(Mode mode);

    void setSpeed(float speed);

    void invert();

    void moveTo(glm::vec3 position);

    void shouldDrawPlayer(bool draw);

    void addTrackablePosition(const glm::vec3 position) {
        trackablePositions.push_back(position);
    }

    void addTrackableEntity(const std::shared_ptr<Entity> &entity) {
        if (entity == nullptr) {
            return;
        }

        if (std::ranges::find(trackableEntities, entity) != trackableEntities.end()) {
            return;
        }

        if (entity.get() == this) {
            return;
        }

        trackableEntities.push_back(entity);
    }

    void clearTrackablePositions() {
        trackablePositions.clear();
    }

    [[nodiscard]] auto getPoints() const -> const std::vector<glm::vec3> & {
        return points;
    }

    [[nodiscard]] auto getLaps() const -> std::size_t {
        return spline.getLaps();
    }

    static void Interface();

    void setBroken(const bool broken) {
        isBroken = broken;
    }

    [[nodiscard]] auto hasBroke() const -> bool {
        return isBroken;
    }

private:
    Mode mode = Mode::AUTO;

    float speed = 1.0F;

    Physics::Spline spline;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> trackablePositions;
    std::vector<std::shared_ptr<Entity> > trackableEntities;

    std::unique_ptr<Model> person;

    static float coneRadius;
    static float coneHeight;
    static bool paused;
    static float trackingDistance;
    static float ventureDistance;

    bool isBroken = false;
    float brokenTime = 0.0F;

    bool drawPlayer = true;
};

#endif