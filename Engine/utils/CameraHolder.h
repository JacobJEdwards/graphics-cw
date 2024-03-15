#include "imgui/imgui.h"
#include "utils/Camera.h"
#include <memory>
#include <string>
#include <unordered_map>

class CameraHolder {
public:
  CameraHolder() : activeCamera(nullptr) {}

  void addCamera(const std::string &name, std::shared_ptr<Camera> camera) {
    cameras[name] = camera;

    if (activeCamera == nullptr) {
      activeCamera = camera;
    }
  }

  void setActiveCamera(const std::string &name) {
    activeCamera = cameras[name];
  }

  void removeCamera(const std::string &name) { cameras.erase(name); }

  std::shared_ptr<Camera> getActiveCamera() { return activeCamera; }

  std::shared_ptr<Camera> getCamera(const std::string &name) {
    return cameras[name];
  }

  void setAspect(float aspect) {
    for (auto &camera : cameras) {
      camera.second->setAspect(aspect);
    }
  }

  void interface() {
    ImGui::Begin("Camera");
    for (auto &camera : cameras) {
      const std::string &name = camera.first;
      bool selected = activeCamera == camera.second;
      if (ImGui::RadioButton(name.c_str(), selected)) {
        activeCamera = camera.second;
      }
    }

    ImGui::End();
  }

private:
  std::shared_ptr<Camera> activeCamera;

  std::unordered_map<std::string, std::shared_ptr<Camera>> cameras;
};
