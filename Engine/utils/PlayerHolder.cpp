#include "PlayerHolder.h"

#include "imgui/imgui.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <utility>

PlayerHolder::~PlayerHolder() { players.clear(); }

void PlayerHolder::draw(const glm::mat4 &view, const glm::mat4 &projection) {
  for (const auto &[name, player] : players) {
    bool isCurrent = player == currentPlayer;
    player->draw(view, projection, !isCurrent);
  }
}

void PlayerHolder::update(float dt) {
  for (const auto &[name, player] : players) {
    player->update(dt);
  }
}

void PlayerHolder::add(const std::string& name, std::shared_ptr<Player> player) {
  players[name] = std::move(player);

  if (players.size() == 1) {
    setCurrent(name);
  }
}

auto PlayerHolder::get(const std::string& name) -> std::shared_ptr<Player> {
  auto it = players.find(name);

  return it != players.end() ? it->second : nullptr;
}

auto PlayerHolder::getCurrent() -> std::shared_ptr<Player> { return currentPlayer; }

void PlayerHolder::setCurrent(const std::string& name) {
  auto it = players.find(name);
  if (it != players.end()) {
    currentPlayer = it->second;
  }
}

void PlayerHolder::setCurrent(std::shared_ptr<Player> player) {
  currentPlayer = std::move(player);
}

auto PlayerHolder::getCurrentName() -> std::string {
  auto it =
      std::find_if(players.begin(), players.end(), [this](const auto &pair) {
        return pair.second == currentPlayer;
      });

  return it != players.end() ? it->first : "";
}

void PlayerHolder::remove(const std::string& name) { players.erase(name); }

void PlayerHolder::remove(const std::shared_ptr<Player>& player) {
  players.erase(
      std::find_if(players.begin(), players.end(), [player](const auto &pair) {
        return pair.second == player;
      }));

  if (currentPlayer == player) {
    currentPlayer = players.begin()->second;
  }
}

void PlayerHolder::clear() { players.clear(); }

/*
ImGui::Begin("Camera");
for (auto &camera : cameras) {
  const std::string &name = camera.first;
  bool selected = activeCamera == camera.second;
  if (ImGui::RadioButton(name.c_str(), selected)) {
    activeCamera = camera.second;
  }
}
*/

void PlayerHolder::interface() {
  ImGui::Begin("Players");
  for (const auto &[name, player] : players) {
    bool selected = currentPlayer == player;
    if (ImGui::RadioButton(name.c_str(), selected)) {
      currentPlayer = player;
    }
  }
  ImGui::End();
}

void PlayerHolder::setAspect(float aspect) {
  for (auto [name, player] : players) {
    player->getCamera().setAspect(aspect);
  }
}
