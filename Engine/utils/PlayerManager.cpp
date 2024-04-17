#include "PlayerManager.h"

#include "imgui/imgui.h"
#include <algorithm>
#include <glm/ext/matrix_float4x4.hpp>
#include <unordered_map>
#include <utility>
#include "renderables/objects/Player.h"
#include "graphics/Shader.h"
#include <memory>
#include <string>
#include <ranges>

std::unordered_map<std::string, std::shared_ptr<Player> > PlayerManager::Players;
std::shared_ptr<Player> PlayerManager::CurrentPlayer;

void PlayerManager::Draw(const glm::mat4 &view, const glm::mat4 &projection) {
    using namespace std::ranges;

    for_each(Players | views::values | views::filter([](const auto &player) {
                 return player != CurrentPlayer && player->shouldDraw() && player->getMode() != Player::Mode::DRIVE &&
                        player->getMode() != Player::Mode::PATH;
             }), [&](const auto &player) {
                 player->draw(view, projection);
             });
}

void PlayerManager::Draw(std::shared_ptr<Shader> shader) {
    using namespace std::ranges;

    for_each(Players | views::values | views::filter([](const auto &player) {
                 return player != CurrentPlayer && player->shouldDraw() && player->getMode() != Player::Mode::DRIVE &&
                        player->getMode() != Player::Mode::PATH;
             }), [&](const auto &player) {
                 player->draw(shader);
             });
}

void PlayerManager::Update(float deltaTime) {
    std::ranges::for_each(Players | std::views::values, [&](const auto &player) {
        player->update(deltaTime);
    });
}

void PlayerManager::Add(const std::string &name, const std::shared_ptr<Player> &player) {
    Players[name] = player;

    if (Players.size() == 1) {
        SetCurrent(name);
    }
}

auto PlayerManager::Get(const std::string &name) -> std::shared_ptr<Player> {
    const auto it = Players.find(name);

    return it != Players.end() ? it->second : nullptr;
}

auto PlayerManager::GetAll() -> std::unordered_map<std::string, std::shared_ptr<Player> > & {
    return Players;
}

auto PlayerManager::GetCurrent() -> std::shared_ptr<Player> { return CurrentPlayer; }

void PlayerManager::SetCurrent(const std::string &name) {
    if (CurrentPlayer) {
        CurrentPlayer->shouldDraw(true);
    }

    if (const auto it = Players.find(name); it != Players.end()) {
        CurrentPlayer = it->second;
        if (CurrentPlayer) {
            CurrentPlayer->shouldDraw(false);
        }
    }
}

void PlayerManager::SetCurrent(std::shared_ptr<Player> player) {
    CurrentPlayer = std::move(player);
}

auto PlayerManager::GetCurrentName() -> std::string {
    const auto it =
            std::ranges::find_if(Players, [&](const auto &pair) {
                return pair.second == CurrentPlayer;
            });

    return it != Players.end() ? it->first : "";
}

void PlayerManager::Remove(const std::string &name) { Players.erase(name); }

void PlayerManager::Remove(const std::shared_ptr<Player> &player) {
    Players.erase(
        std::ranges::find_if(Players, [player](const auto &pair) {
            return pair.second == player;
        }));

    if (CurrentPlayer == player) {
        CurrentPlayer = Players.begin()->second;
    }
}

void PlayerManager::Clear() { Players.clear(); }

void PlayerManager::Interface() {
    using namespace std::ranges;
    ImGui::Begin("Players");

    for (const auto &[name, player]: Players) {
        if (ImGui::RadioButton(name.c_str(), CurrentPlayer == player)) {
            CurrentPlayer = player;
        }
    }
    ImGui::End();
}

void PlayerManager::SetAspect(float aspect) {
    using namespace std::ranges;

    for_each(Players | views::values, [&](const auto &player) {
        player->getCamera().setAspect(aspect);
    });
}
