#include "PlayerManager.h"

#include "imgui/imgui.h"
#include <algorithm>
#include <utility>

std::unordered_map<std::string, std::shared_ptr<Player>> PlayerManager::Players;
std::shared_ptr<Player> PlayerManager::CurrentPlayer;

void PlayerManager::Draw(const glm::mat4 &view, const glm::mat4 &projection, bool depthPass) {
    for (const auto &[name, player]: Players) {
        bool isCurrent = player == CurrentPlayer;
        player->draw(view, projection, !isCurrent || depthPass);
    }
}

void PlayerManager::Update(float dt) {
    for (const auto &[name, player]: Players) {
        player->update(dt);
    }
}

void PlayerManager::Add(const std::string &name, std::shared_ptr<Player> player) {
    Players[name] = std::move(player);

    if (Players.size() == 1) {
        SetCurrent(name);
    }
}

auto PlayerManager::Get(const std::string &name) -> std::shared_ptr<Player> {
    auto it = Players.find(name);

    return it != Players.end() ? it->second : nullptr;
}

auto PlayerManager::GetAll() -> std::unordered_map<std::string, std::shared_ptr<Player>> & {
    return Players;
}

auto PlayerManager::GetCurrent() -> std::shared_ptr<Player> { return CurrentPlayer; }

void PlayerManager::SetCurrent(const std::string &name) {
    if (CurrentPlayer) {
        CurrentPlayer->setDrawModel(true);
    }

    auto it = Players.find(name);
    if (it != Players.end()) {
        CurrentPlayer = it->second;
        if (CurrentPlayer) {
            CurrentPlayer->setDrawModel(false);
        }
    }

}

void PlayerManager::SetCurrent(std::shared_ptr<Player> player) {
    CurrentPlayer = std::move(player);
}

auto PlayerManager::GetCurrentName() -> std::string {
    auto it =
            std::find_if(Players.begin(), Players.end(), [&](const auto &pair) {
                return pair.second == CurrentPlayer;
            });

    return it != Players.end() ? it->first : "";
}

void PlayerManager::Remove(const std::string &name) { Players.erase(name); }

void PlayerManager::Remove(const std::shared_ptr<Player> &player) {
    Players.erase(
            std::find_if(Players.begin(), Players.end(), [player](const auto &pair) {
                return pair.second == player;
            }));

    if (CurrentPlayer == player) {
        CurrentPlayer = Players.begin()->second;
    }
}

void PlayerManager::Clear() { Players.clear(); }

void PlayerManager::Interface() {
    ImGui::Begin("Players");
    for (const auto &[name, player]: Players) {
        bool selected = CurrentPlayer == player;
        if (ImGui::RadioButton(name.c_str(), selected)) {
            CurrentPlayer = player;
        }
    }
    ImGui::End();
}

void PlayerManager::SetAspect(float aspect) {
    for (auto [name, player]: Players) {
        player->getCamera().setAspect(aspect);
    }
}
