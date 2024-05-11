//
// Created by Jacob Edwards on 01/05/2024.
//

#include "Scene.h"


#include "Walls.h"
#include "graphics/Shader.h"
#include "renderables/objects/ProceduralTerrain.h"
#include "renderables/objects/FerrisWheel.h"
#include "renderables/Renderable.h"
#include "renderables/objects/Skybox.h"
#include "renderables/objects/RollerCoaster.h"
#include "renderables/objects/Barriers.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>


Scene::Scene() {
    terrain = std::make_shared<ProceduralTerrain>();
    ferrisWheel = std::make_shared<FerrisWheel>();
    rollerCoaster = std::make_shared<RollerCoaster>();
    skybox = std::make_shared<Skybox>();
    walls = std::make_shared<Walls>();
    barriers = std::make_shared<Barriers>();
}

void Scene::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    terrain->draw(view, projection);
    ferrisWheel->draw(view, projection);
    rollerCoaster->draw(view, projection);
    barriers->draw(view, projection);
    skybox->draw(view, projection);
}

void Scene::draw(const std::shared_ptr<Shader> shader) const {
    terrain->draw(shader);
    ferrisWheel->draw(shader);
    rollerCoaster->draw(shader);
    barriers->draw(shader);
    skybox->draw(shader);
}

void Scene::update(const float deltaTime) const {
    ferrisWheel->update(deltaTime);
    skybox->update(deltaTime);
}

auto Scene::getTerrain() -> std::shared_ptr<ProceduralTerrain> {
    return terrain;
}

auto Scene::getSkybox() -> std::shared_ptr<Skybox> {
    return skybox;
}

auto Scene::getFerrisWheel() -> std::shared_ptr<FerrisWheel> {
    return ferrisWheel;
}

auto Scene::getRollerCoaster() -> std::shared_ptr<RollerCoaster> {
    return rollerCoaster;
}

auto Scene::getWalls() -> std::shared_ptr<Walls> {
    return walls;
}

auto Scene::getBarriers() -> std::shared_ptr<Barriers> {
    return barriers;
}
