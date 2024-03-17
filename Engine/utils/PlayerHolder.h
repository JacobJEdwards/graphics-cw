#ifndef PLAYERHOLDER_H
#define PLAYERHOLDER_H

#include "utils/Objects/Player.h"
#include <glm/glm.hpp>
#include <memory.h>
#include <string>
#include <unordered_map>

class PlayerHolder {
public:
  PlayerHolder() = default;
  ~PlayerHolder();

  void draw(const glm::mat4 &view, const glm::mat4 &projection);
  void update(float dt);

  void add(std::string name, std::shared_ptr<Player> player);
  std::shared_ptr<Player> get(std::string name);
  std::shared_ptr<Player> getCurrent();
  void setCurrent(std::string name);
  void setCurrent(std::shared_ptr<Player> player);
  std::string getCurrentName();
  void remove(std::string name);
  void remove(std::shared_ptr<Player> player);
  void clear();

  void setAspect(float aspect);

  void interface();

private:
  std::unordered_map<std::string, std::shared_ptr<Player>> players;
  std::shared_ptr<Player> currentPlayer;
};

#endif
