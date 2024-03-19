#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include "graphics/FrameBuffer.h"
#include "shapes/Plane.h"
#include "utils/Shader.h"
#include <memory>

class PostProcess {
public:
  PostProcess(int width, int height, std::shared_ptr<Shader> shader,
              bool multisampled = true);
  void render();
  void begin();
  void end();
  void setShader(std::shared_ptr<Shader> shader);
  std::shared_ptr<Shader> getShader();
  std::shared_ptr<FrameBuffer> getFrameBuffer();
  void setWidth(int width);
  void setHeight(int height);
  void resize(int width, int height);

private:
  std::shared_ptr<Shader> shader;
  std::shared_ptr<FrameBuffer> frameBuffer;

  Plane renderPlane;

  int width;
  int height;
};

#endif // POSTPROCESS_H
