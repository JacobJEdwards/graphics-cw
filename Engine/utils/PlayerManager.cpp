#include "PlayerManager.h"

#include "imgui/imgui.h"
#include <algorithm>
#include <glm/ext/matrix_float4x4.hpp>
#include <unordered_map>
#include <utility>
#include "renderables/objects/BumperCar.h"
#include "renderables/objects/Player.h"
#include "graphics/Shader.h"
#include <memory>
#include <print>
#include <string>
#include <ranges>


void PlayerManager::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    using namespace std::ranges;

    for_each(players | views::values | views::filter([&](const auto &player) {
                 return player != currentPlayer;
             }), [&](const auto &player) {
                 player->draw(view, projection);
             });
}

void PlayerManager::draw(std::shared_ptr<Shader> shader) const {
    using namespace std::ranges;

    for_each(players | views::values | views::filter([&](const auto &player) {
                 return player != currentPlayer;
             }), [&](const auto &player) {
                 player->draw(shader);
             });
}

void PlayerManager::draw() const {
    using namespace std::ranges;

    for_each(players | views::values | views::filter([&](const auto &player) {
                 return player != currentPlayer;
             }), [&](const auto &player) {
                 player->draw();
             });
}

void PlayerManager::update(float deltaTime) {
    std::ranges::for_each(players | std::views::values, [&](const auto &player) {
        player->update(deltaTime);
    });
}

void PlayerManager::add(const std::string &name, const std::shared_ptr<Player> &player) {
    players[name] = player;

    if (players.size() == 1) {
        setCurrent(name);
    }
}

auto PlayerManager::get(const std::string &name) -> std::shared_ptr<Player> {
    const auto it = players.find(name);

    return it != players.end() ? it->second : nullptr;
}

auto PlayerManager::getAll() -> std::unordered_map<std::string, std::shared_ptr<Player> > & {
    return players;
}

auto PlayerManager::getCurrent() -> std::shared_ptr<Player> { return currentPlayer; }

void PlayerManager::setCurrent(const std::string &name) {
    if (const auto it = players.find(name); it != players.end()) {
        currentPlayer = it->second;
        if (currentPlayer) {
            if (currentPlayer->getMode() == Player::Mode::DRIVE) {
                currentPlayer->getCar()->setMode(BumperCar::Mode::PLAYER);
                currentPlayer->getCar()->isCurrentPlayer(true);
            }

            if (currentPlayer->getMode() == Player::Mode::PATH) {
                currentPlayer->getCar()->isCurrentPlayer(true);
            }
        }
    }

    for (const auto &[key, player]: players) {
        if (key != name) {
            if (player->getMode() == Player::Mode::DRIVE) {
                player->getCar()->setMode(BumperCar::Mode::AUTO);
                player->getCar()->isCurrentPlayer(false);
            }

            if (player->getMode() == Player::Mode::PATH) {
                player->getCar()->isCurrentPlayer(false);
            }
        }
    }
}

void PlayerManager::setCurrent(std::shared_ptr<Player> player) {
    currentPlayer = std::move(player);
}

auto PlayerManager::getCurrentName() -> std::string {
    const auto it =
            std::ranges::find_if(players, [&](const auto &pair) {
                return pair.second == currentPlayer;
            });

    return it != players.end() ? it->first : "";
}

void PlayerManager::remove(const std::string &name) {
    players.erase(name);
}

void PlayerManager::remove(const std::shared_ptr<Player> &player) {
    players.erase(
        std::ranges::find_if(players, [player](const auto &pair) {
            return pair.second == player;
        }));

    if (currentPlayer == player) {
        currentPlayer = players.begin()->second;
    }
}

void PlayerManager::clear() {
    players.clear();
}

void PlayerManager::interface() {
    ImGui::Begin("Players");

    for (const auto &[name, player]: players) {
        if (ImGui::RadioButton(name.c_str(), currentPlayer == player)) {
            setCurrent(name);
        }
    }

    ImGui::End();
}

void PlayerManager::setAspect(float aspect) {
    using namespace std::ranges;

    for_each(players | views::values, [&](const auto &player) {
        player->getCamera().setAspect(aspect);
    });
}
