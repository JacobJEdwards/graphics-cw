//
// Created by Jacob Edwards on 22/02/2024.
//

#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <unordered_map>
#include <memory>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"
#include "./Texture.h"
#include "../utils/Shader.h"


class Model {
    using MeshPtr = std::unique_ptr<Mesh>;

public:
    explicit Model(const std::string &path);
    void draw(const Shader *shader) const;
    [[nodiscard]] auto detectCollisions(const glm::vec3 &position) const -> bool;
    auto getCentre() const -> glm::vec3;
    auto getOffset(const glm::vec3 &point) const -> glm::vec3;

private:

    std::unordered_map<std::string, Texture::Data> textures_loaded;
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::string directory;

    void loadModel(const std::string &path);

    void processNode(const aiNode * node, const aiScene *scene);

    auto processMesh(aiMesh *mesh, const aiScene *scene) -> Mesh;

    auto loadMaterialTextures(const aiMaterial * mat, aiTextureType type, Texture::Type texType) -> std::vector<Texture::Data>;

};


#endif //MODEL_H
