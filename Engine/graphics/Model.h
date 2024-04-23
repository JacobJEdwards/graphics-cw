//
// Created by Jacob Edwards on 22/02/2024.
//

#ifndef MODEL_H
#define MODEL_H

#include <assimp/material.h>
#include <assimp/mesh.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"
#include "graphics/Texture.h"
#include "physics/ModelAttributes.h"
#include "utils/BoundingBox.h"
#include "graphics/Shader.h"
#include "renderables/Renderable.h"

class Model final : public Renderable {
public:
    Physics::Attributes attributes;

    explicit Model(const std::filesystem::path &path);

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    void draw(std::shared_ptr<Shader> shader) const override;

    [[nodiscard]] auto getBoundingBox() const -> BoundingBox;

private:
    std::unordered_map<std::string, Texture::Data> texturesLoaded;
    std::vector<std::unique_ptr<Mesh> > meshes;
    std::filesystem::path directory;
    std::shared_ptr<BoundingBox> boundingBox;

    void loadModel(const std::filesystem::path &path);

    void processNode(const aiNode *node, const aiScene *scene);

    auto processMesh(aiMesh *mesh, const aiScene *scene) -> Mesh;

    auto loadMaterialTextures(const aiMaterial *mat, aiTextureType type,
                              Texture::Type texType)
        -> std::vector<Texture::Data>;
};

#endif // MODEL_H
