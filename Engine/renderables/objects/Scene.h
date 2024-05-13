//
// Created by Jacob Edwards on 01/05/2024.
//

#ifndef SCENE_H
#define SCENE_H

#include "Walls.h"
#include "graphics/Shader.h"
#include "renderables/objects/ProceduralTerrain.h"
#include "renderables/objects/FerrisWheel.h"
#include "renderables/Renderable.h"
#include "renderables/objects/Skybox.h"
#include "renderables/objects/RollerCoaster.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include "renderables/objects/Spotlight.h"

#include "Barriers.h"
#include "Lights.h"

class Scene final : public Renderable {
public:
    using Renderable::draw;

    Scene();

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    void draw(std::shared_ptr<Shader> shader) const override;

    void draw() const override;

    void update(float deltaTime) const;

    auto getTerrain() -> std::shared_ptr<ProceduralTerrain>;

    auto getSkybox() -> std::shared_ptr<Skybox>;

    auto getFerrisWheel() -> std::shared_ptr<FerrisWheel>;

    auto getRollerCoaster() -> std::shared_ptr<RollerCoaster>;

    auto getWalls() -> std::shared_ptr<Walls>;

    auto getBarriers() -> std::shared_ptr<Barriers>;

    auto getLightObjects() -> std::shared_ptr<LightObjects>;

private:
    std::shared_ptr<ProceduralTerrain> terrain;
    std::shared_ptr<FerrisWheel> ferrisWheel;
    std::shared_ptr<RollerCoaster> rollerCoaster;
    std::shared_ptr<Skybox> skybox;
    std::shared_ptr<Walls> walls;
    std::shared_ptr<Barriers> barriers;
    std::shared_ptr<LightObjects> lightObjects;
};


#endif //SCENE_H
