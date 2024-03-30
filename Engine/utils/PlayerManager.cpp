#include "PlayerManager.h"

#include "imgui/imgui.h"
#include <algorithm>
#include <unordered_map>
#include <utility>

std::unordered_map<std::string, std::shared_ptr<Player>> PlayerManager::Players;
std::shared_ptr<Player> PlayerManager::CurrentPlayer;

void PlayerManager::Draw(const glm::mat4 &view, const glm::mat4 &projection) {
    for (const auto &[name, player]: Players) {
        const bool isCurrent = player == CurrentPlayer;

        if (!isCurrent && player->shouldDraw()) {
            player->draw(view, projection);
        }
    }
}

void PlayerManager::Draw(std::shared_ptr<Shader> shader) {
    for (const auto &[name, player]: Players) {
        if (player->shouldDraw()) {
            player->draw(shader);
        }
    }


}

void PlayerManager::Update(float deltaTime) {
    for (const auto &[name, player]: Players) {
        player->update(deltaTime);
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
        CurrentPlayer->shouldDraw(true);
    }

    auto it = Players.find(name);

    if (it != Players.end()) {
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
        const bool selected = CurrentPlayer == player;
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
