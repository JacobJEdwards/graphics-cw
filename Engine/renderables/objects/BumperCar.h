#ifndef BUMPERCAR_H
#define BUMPERCAR_H

#include "graphics/Model.h"
#include "physics/Spline.h"
#include "graphics/Shader.h"
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <vector>
#include "renderables/Entity.h"
#include "utils/Lights.h"

class BumperCar final : public Entity {
public:
    using Entity::draw;

    enum class Mode {
        PATHED,
        TRACK,
        AUTO,
        NONE,
        FOLLOW,
        RANDOM,
        PLAYER
    };

    explicit BumperCar(glm::vec2 centre = {0.0F, 0.0F}, float radius = 50.0F, float speed = 2.0F);

    void update(float deltaTime) override;

    void reset();

    void generateTexture();

    void draw(std::shared_ptr<Shader> shader) const override;

    void setMode(Mode mode);

    void setSpeed(float speed);

    void invert();

    void moveTo(glm::vec3 position);

    void shouldDrawPlayer(bool draw);

    void addTrackablePosition(glm::vec3 position);

    void addTrackableEntity(const std::shared_ptr<Entity> &entity);

    void clearTrackablePositions();

    void setPersonShader(const std::shared_ptr<Shader> &shader);

    [[nodiscard]] auto getPoints() const -> const std::vector<glm::vec3> &;

    [[nodiscard]] auto getLaps() const -> std::size_t;

    static void Interface();

    void takeDamage(float damage);

    [[nodiscard]] auto hasBroke() const -> bool;

    void setIsPlayer(bool isPlayer);

    void isCurrentPlayer(bool isCurrentPlayer);

    void setNitro(bool nitro);

    void collisionResponse() override;

    [[nodiscard]] auto getMode() const -> Mode;

    [[nodiscard]] auto hasExploded() const -> bool;

    [[nodiscard]] auto getPointLight() const -> PointLight;

    [[nodiscard]] auto isOnFire() const -> bool;

    void isThirdPerson(bool thirdPerson);

private:
    Mode mode = Mode::AUTO;

    float speed = 1.0F;
    Physics::Spline::Type splineType = Physics::Spline::Type::CATMULLROM;

    Physics::Spline spline;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> trackablePositions;
    std::vector<std::shared_ptr<Entity> > trackableEntities;

    Model person;
    std::shared_ptr<Shader> personShader;

    Texture::Data damageTexture;

    static float coneRadius;
    static float coneHeight;
    static bool paused;
    static float trackingDistance;
    static float ventureDistance;

    // between 0 and 1
    float damageTaken = 0.0F;

    bool thirdPerson = false;

    bool isBroken = false;
    float brokenTime = 0.0F;

    bool isExploding = false;
    float explodeTime = 0.0F;

    bool drawPlayer = true;

    bool isPlayer = false;
    bool currentPlayer = false;

    float nitroDuration = 0.0F;
    float nitroMaxDuration = 5.0F;
    bool nitroActive = false;
    float nitroForce = 250.0F;
};

#endif
