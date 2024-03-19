#ifndef BUMPERCAR_H
#define BUMPERCAR_H

#include "graphics/Model.h"
#include "physics/Spline.h"
#include "utils/Shader.h"
#include <memory>

class BumperCar {
public:
  BumperCar(std::shared_ptr<Shader> shader) { model.setShader(shader); }
  void draw() { model.draw(); }

  void calculateForce(const Physics::Spline &spline, float t, float dt) {}

  void update(float dt) { model.update(dt); }

  Model model = Model("../Assets/objects/helecopter/chopper.obj");
};

#endif
