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
#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"
#include "utils/Shader.h"

class Model {
public:
    Physics::Attributes attributes;

    explicit Model(const std::filesystem::path &path);

    void draw(const glm::mat4 &view, const glm::mat4 &projection, bool depthPass = false);

    [[nodiscard]] auto detectCollisions(const glm::vec3 &position) const -> bool;

    [[nodiscard]] auto isColliding(const BoundingBox &other) const -> bool;

    [[nodiscard]] auto isColliding(const Model &other) const -> bool {
        for (const auto &mesh: meshes) {
            for (const auto &otherMesh: other.meshes) {
                if (mesh->isColliding(otherMesh->getBoundingBox())) {
                    return true;
                }
            }
        }
        return false;
    }

    [[nodiscard]] auto getCentre() const -> glm::vec3;

    [[nodiscard]] auto getOffset(const glm::vec3 &point) const -> glm::vec3;

    [[nodiscard]] auto getOffset(const BoundingBox &other) const -> glm::vec3;

    void setShader(std::shared_ptr<Shader> shader);

    void translate(const glm::vec3 &translation);

    void transform(const glm::mat4 &transform);

    void scale(const glm::vec3 &scale);

    void rotate(const glm::vec3 &axis, float angle);

    [[nodiscard]] auto getBoundingBox() const -> BoundingBox;

    void update(float dt);

private:
    std::unordered_map<std::string, Texture::Data> textures_loaded;
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::filesystem::path directory;

    std::shared_ptr<Shader> shader;

    BoundingBox box;
    glm::mat4 modelMatrix = Config::IDENTITY_MATRIX;

    void loadModel(const std::filesystem::path &path);

    void processNode(const aiNode *node, const aiScene *scene);

    auto processMesh(aiMesh *mesh, const aiScene *scene) -> Mesh;

    auto loadMaterialTextures(const aiMaterial *mat, aiTextureType type,
                              Texture::Type texType)
    -> std::vector<Texture::Data>;

    void calculateBoundingBox();
};

#endif // MODEL_H