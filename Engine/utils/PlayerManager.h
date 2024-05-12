#ifndef PLAYERHOLDER_H
#define PLAYERHOLDER_H

#include "graphics/Shader.h"
#include "renderables/Renderable.h"
#include "renderables/objects/Player.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <memory.h>
#include <memory>
#include <string>
#include <unordered_map>
#include "utils/Singleton.h"

class PlayerManager final : public Singleton<PlayerManager>, public Renderable {
    friend class Singleton;

public:
    explicit PlayerManager(Token) {
    }

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    void draw(std::shared_ptr<Shader> shader) const override;

    void draw() const override;

    void update(float deltaTime);

    void add(const std::string &name, const std::shared_ptr<Player> &player);

    auto get(const std::string &name) -> std::shared_ptr<Player>;

    auto getAll() -> std::unordered_map<std::string, std::shared_ptr<Player> > &;

    auto getCurrent() -> std::shared_ptr<Player>;

    void setCurrent(const std::string &name);

    void setCurrent(std::shared_ptr<Player> player);

    auto getCurrentName() -> std::string;

    void remove(const std::string &name);

    void remove(const std::shared_ptr<Player> &player);

    void clear();

    void setAspect(float aspect);

    void interface();

private:
    std::unordered_map<std::string, std::shared_ptr<Player> > players;
    std::shared_ptr<Player> currentPlayer;
};

#endif
