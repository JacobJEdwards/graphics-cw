//
// Created by Jacob Edwards on 01/05/2024.
//

#ifndef SCENE_H
#define SCENE_H

#include "renderables/objects/ProceduralTerrain.h"
#include "renderables/Entity.h"
#include "renderables/objects/FerrisWheel.h"
#include "renderables/Renderable.h"
#include "renderables/objects/Skybox.h"
#include "renderables/objects/RollerCoaster.h"

class Scene final : public Renderable {
public:
    using Renderable::draw;

    Scene() = default;

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override {
        terrain.draw(view, projection);
        ferrisWheel.draw(view, projection);
        rollerCoaster.draw(view, projection);
        skybox.draw(view, projection);
    }

private:
    ProceduralTerrain terrain;
    FerrisWheel ferrisWheel;
    RollerCoaster rollerCoaster;
    Skybox skybox;
};


#endif //SCENE_H
