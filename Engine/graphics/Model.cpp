//
// Created by Jacob Edwards on 22/02/2024.
//

#include "Model.h"

#include <algorithm>
#include <assimp/material.h>
#include <assimp/mesh.h>
#include <glm/common.hpp>
#include <limits>
#include <memory>
#include <numeric>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "graphics/Mesh.h"
#include "graphics/Texture.h"
#include "helpers/AssimpGLMHelpers.h"
#include "physics/Gravity.h"
#include "utils/BoundingBox.h"
#include "utils/Shader.h"
#include "utils/Vertex.h"
#include "utils/ShaderManager.h"
#include "App.h"

Model::Model(const std::filesystem::path &path) { loadModel(path); }

void Model::draw(std::shared_ptr<Shader> shader) const {
    for (const auto &mesh: meshes) {
        mesh->draw(shader);
    }
}

void Model::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    shader->use();

    shader->setUniform("model", attributes.transform);
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);

    for (const auto &mesh: meshes) {
        mesh->draw(shader);
    }
}

void Model::loadModel(const std::filesystem::path &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
            path.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                           aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
                           aiProcess_GenBoundingBoxes);

    if ((scene == nullptr) ||
        ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0U) ||
        (scene->mRootNode == nullptr)) {
        throw std::runtime_error(importer.GetErrorString());
    }

    directory = path.parent_path();

    processNode(scene->mRootNode, scene);

    calculateBoundingBox();

    attributes.position = getCentre();
    attributes.transform = glm::mat4(1.0F);
}

void Model::processNode(const aiNode *const node, const aiScene *scene) {
    std::stack<const aiNode *> nodeStack;
    nodeStack.push(node);

    while (!nodeStack.empty()) {
        const aiNode *currentNode = nodeStack.top();
        nodeStack.pop();

        for (unsigned int i = 0; i < currentNode->mNumMeshes; ++i) {
            aiMesh *mesh = scene->mMeshes[currentNode->mMeshes[i]];
            meshes.push_back(std::make_unique<Mesh>(processMesh(mesh, scene)));
        }

        for (unsigned int i = 0; i < currentNode->mNumChildren; i++) {
            nodeStack.push(currentNode->mChildren[i]);
        }
    }
}

auto Model::processMesh(aiMesh *mesh, const aiScene *scene) -> Mesh {
    std::vector<Vertex::Data> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture::Data> textures;

    const BoundingBox box{AssimpGLMHelpers::getGLMVec(mesh->mAABB.mMin),
                          AssimpGLMHelpers::getGLMVec(mesh->mAABB.mMax)};

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex::Data vertex{};

        vertex.position = AssimpGLMHelpers::getGLMVec(mesh->mVertices[i]);

        if (mesh->HasNormals()) {
            vertex.normal = AssimpGLMHelpers::getGLMVec(mesh->mNormals[i]);
        }
        if (mesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
        {
            // a vertex can contain up to 8 different texture coordinates. We thus
            // make the assumption that we won't use models where a vertex can have
            // multiple texture coordinates so we always take the first set (0).
            const glm::vec3 vec =
                    AssimpGLMHelpers::getGLMVec(mesh->mTextureCoords[0][i]);
            vertex.texCoords = glm::vec2(vec.x, vec.y);

            if (mesh->HasTangentsAndBitangents()) {
                // tangent
                vertex.tangent = AssimpGLMHelpers::getGLMVec(mesh->mTangents[i]);
                // bitangent
                vertex.bitangent = AssimpGLMHelpers::getGLMVec(mesh->mBitangents[i]);
            }
        } else {
            vertex.texCoords = glm::vec2(0.0F, 0.0F);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    std::vector<Texture::Data> diffuseMaps = loadMaterialTextures(
            material, aiTextureType_DIFFUSE, Texture::Type::DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture::Data> specularMaps = loadMaterialTextures(
            material, aiTextureType_SPECULAR, Texture::Type::SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. height maps
    std::vector<Texture::Data> heightMaps = loadMaterialTextures(
            material, aiTextureType_HEIGHT, Texture::Type::HEIGHT);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    // 4. normal maps
    std::vector<Texture::Data> normalMaps = loadMaterialTextures(
            material, aiTextureType_NORMALS, Texture::Type::NORMAL);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 5. ambient occlusion maps
    std::vector<Texture::Data> ambientOcclusionMaps = loadMaterialTextures(
            material, aiTextureType_AMBIENT, Texture::Type::AMBIENT_OCCLUSION);
    textures.insert(textures.end(), ambientOcclusionMaps.begin(),
                    ambientOcclusionMaps.end());
    // 6. emissive maps
    std::vector<Texture::Data> emissiveMaps = loadMaterialTextures(
            material, aiTextureType_EMISSIVE, Texture::Type::EMISSIVE);
    textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    return {vertices, indices, textures, box};
}

auto Model::loadMaterialTextures(const aiMaterial *const mat,
                                 const aiTextureType type,
                                 const Texture::Type texType)
-> std::vector<Texture::Data> {
    std::vector<Texture::Data> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);

        const std::string path(str.C_Str());

        auto loaded = textures_loaded.find(path);

        if (loaded != textures_loaded.end()) {
            textures.push_back(loaded->second);
            continue;
        }

        const Texture::Data texture{Texture::Loader::load(path, directory), texType,
                                    str.C_Str()};

        textures.push_back(texture);
        textures_loaded[path] = texture;
    }

    return textures;
}

auto Model::detectCollisions(const glm::vec3 &point) const -> bool {
    return std::ranges::any_of(
            meshes, [&](const auto &mesh) { return mesh->detectCollisions(point); });
}

auto Model::getCentre() const -> glm::vec3 {
    return std::accumulate(meshes.begin(), meshes.end(), glm::vec3(0.0F),
                           [](const auto &acc, const auto &mesh) {
                               return acc + mesh->getCentre();
                           }) /
           static_cast<float>(meshes.size());
}

auto Model::getOffset(const glm::vec3 &point) const -> glm::vec3 {
    return std::accumulate(meshes.begin(), meshes.end(), glm::vec3(0.0F),
                           [&](const auto &acc, const auto &mesh) {
                               return acc + mesh->getOffset(point);
                           }) /
           static_cast<float>(meshes.size());
}

auto Model::getOffset(const BoundingBox &other) const -> glm::vec3 {
    return std::accumulate(meshes.begin(), meshes.end(), glm::vec3(0.0F),
                           [&](const auto &acc, const auto &mesh) {
                               return acc + mesh->getOffset(other);
                           }) /
           static_cast<float>(meshes.size());
}

[[nodiscard]] auto Model::isColliding(const BoundingBox &other) const -> bool {
    return std::ranges::any_of(
            meshes, [&](const auto &mesh) { return mesh->isColliding(other); });
}

void Model::rotate(const glm::vec3 &axis, float angle) {
    attributes.applyRotation(axis * angle);

    for (auto &mesh: meshes) {
        mesh->rotate(axis, angle);
    }

    box.rotate(axis, angle);
}

void Model::translate(const glm::vec3 &translation) {
    attributes.position += translation;

    for (auto &mesh: meshes) {
        mesh->translate(translation);
    }

    box.translate(translation);
}

void Model::scale(const glm::vec3 &scale) {
    attributes.transform = glm::scale(attributes.transform, scale);

    for (auto &mesh: meshes) {
        mesh->scale(scale);
    }

    box.scale(scale);
}

void Model::transform(const glm::mat4 &transform) {
    attributes.transform = transform * attributes.transform;

    for (auto &mesh: meshes) {
        mesh->transform(transform);
    }

    attributes.position =
            glm::vec3(attributes.transform * glm::vec4(attributes.position, 1.0F));

    box.transform(transform);
}

void Model::calculateBoundingBox() {
    auto min = glm::vec3(std::numeric_limits<float>::max());
    auto max = glm::vec3(std::numeric_limits<float>::min());

    std::vector<std::unique_ptr<BoundingBox>> children(meshes.size());

    for (const auto &mesh: meshes) {
        auto meshBox = mesh->getBoundingBox();
        auto [meshMin, meshMax] = meshBox.getMinMax();

        children.push_back(std::make_unique<BoundingBox>(meshBox));

        min = glm::min(min, meshMin);
        max = glm::max(max, meshMax);
    }

    this->box = BoundingBox(min, max);
    // this->box.addChildren(children);
}

[[nodiscard]] auto Model::getBoundingBox() const -> BoundingBox {
    return box;
}

void Model::update(float deltaTime) {
    const glm::mat4 oldTransform = attributes.transform;

    attributes.update(deltaTime);

    const glm::mat4 newTransform = attributes.transform;

    for (auto &mesh: meshes) {
        mesh->transform(newTransform * glm::inverse(oldTransform));
    }

    box.transform(newTransform * glm::inverse(oldTransform));
}