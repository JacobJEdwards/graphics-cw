#ifndef BUMPERCAR_H
#define BUMPERCAR_H

#include "graphics/Model.h"
#include "physics/Spline.h"
#include "utils/Shader.h"
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <vector>
#include "Entity.h"

class BumperCar : public Entity {
public:

    explicit BumperCar(glm::vec2 centre = {0.0F, 0.0F}, float radius = 1.0F, float speed = 1.0F);

    explicit BumperCar(std::vector<glm::vec3> points, float speed = 1.0F);

    void update(float deltaTime) override;

    void setTrack(bool track);

    void setPathed(bool pathed);

    void setSpeed(float speed);

    void invert();

    void moveTo(glm::vec3 position);

private:
    bool pathed = false;
    bool track = true;

    float speed = 1.0F;

    Physics::Spline spline;
    std::vector<glm::vec3> points;

};

#endif
