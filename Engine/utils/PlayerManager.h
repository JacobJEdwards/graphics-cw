#ifndef PLAYERHOLDER_H
#define PLAYERHOLDER_H

#include "utils/Objects/Player.h"
#include <glm/glm.hpp>
#include <memory.h>
#include <string>
#include <unordered_map>

class PlayerManager {
public:
    PlayerManager() = delete;

    static void Draw(const glm::mat4 &view, const glm::mat4 &projection);

    static void Draw(std::shared_ptr<Shader> shader);

    static void Update(float deltaTime);

    static void Add(const std::string &name, std::shared_ptr<Player> player);

    static auto Get(const std::string &name) -> std::shared_ptr<Player>;

    static auto GetAll() -> std::unordered_map<std::string, std::shared_ptr<Player>> &;

    static auto GetCurrent() -> std::shared_ptr<Player>;

    static void SetCurrent(const std::string &name);

    static void SetCurrent(std::shared_ptr<Player> player);

    static auto GetCurrentName() -> std::string;

    static void Remove(const std::string &name);

    static void Remove(const std::shared_ptr<Player> &player);

    static void Clear();

    static void SetAspect(float aspect);

    static void Interface();


private:
    static std::unordered_map<std::string, std::shared_ptr<Player>> Players;
    static std::shared_ptr<Player> CurrentPlayer;
};

#endif
