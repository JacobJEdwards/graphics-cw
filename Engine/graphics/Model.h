//
// Created by Jacob Edwards on 22/02/2024.
//

#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Config.h"
#include "Mesh.h"
#include "graphics/Texture.h"
#include "utils/BoundingBox.h"
#include "utils/Shader.h"

class Model {
  using MeshPtr = std::unique_ptr<Mesh>;

public:
  explicit Model(const std::string &path);
  void draw(const Shader *shader) const;
  [[nodiscard]] auto detectCollisions(const glm::vec3 &position) const -> bool;
  [[nodiscard]] auto isColliding(const BoundingBox &other) const -> bool;
  auto getCentre() const -> glm::vec3;
  auto getOffset(const glm::vec3 &point) const -> glm::vec3;
  auto getOffset(const BoundingBox &other) const -> glm::vec3;

  void translate(const glm::vec3 &translation);
  void transform(const glm::mat4 &transform);
  void scale(const glm::vec3 &scale);
  void rotate(const glm::vec3 &axis, float angle);

  [[nodiscard]] auto getBoundingBox() const -> BoundingBox;

private:
  std::unordered_map<std::string, Texture::Data> textures_loaded;
  std::vector<std::unique_ptr<Mesh>> meshes;
  std::string directory;

  BoundingBox boundingBox;
  glm::mat4 modelMatrix = Config::IDENTITY_MATRIX;

  void loadModel(const std::string &path);

  void processNode(const aiNode *node, const aiScene *scene);

  auto processMesh(aiMesh *mesh, const aiScene *scene) -> Mesh;

  auto loadMaterialTextures(const aiMaterial *mat, aiTextureType type,
                            Texture::Type texType)
      -> std::vector<Texture::Data>;
};

#endif // MODEL_H
