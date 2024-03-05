//
// Created by Jacob Edwards on 22/02/2024.
//

#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <sstream>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

class Model {
    using MeshPtr = std::unique_ptr<Mesh>;

public:
    explicit Model(const std::string &path, bool gamma = false);
    void draw(const Shader *shader) const;

private:
    BoundingBox boundingBox{ glm::vec3(0.0F), glm::vec3(0.0F) };


    std::unordered_map<std::string, Texture::Data> textures_loaded;
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::string directory;
    bool gammaCorrection;

    void loadModel(const std::string &path);

    void processNode(const aiNode * node, const aiScene *scene);

    auto processMesh(aiMesh *mesh, const aiScene *scene) -> Mesh;

    auto loadMaterialTextures(const aiMaterial * mat, aiTextureType type, Texture::Type texType) -> std::vector<Texture::Data>;

};


#endif //MODEL_H
