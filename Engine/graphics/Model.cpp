//
// Created by Jacob Edwards on 22/02/2024.
//

#include "Model.h"

#include <assimp/material.h>
#include <assimp/mesh.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <sstream>
#include <stack>

#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "utils/Vertex.h"
#include "graphics/Texture.h"
#include "graphics/Mesh.h"
#include "helpers/AssimpGLMHelpers.h"
#include "utils/Shader.h"


Model::Model(const std::string &path, const bool gamma) : gammaCorrection(gamma) {
    loadModel(path);
}

void Model::draw(const Shader *shader) const {
    for (const auto &mesh: meshes) {
        mesh->draw(shader);
    }
}

void Model::loadModel(const std::string &path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);

    if ((scene == nullptr) || ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0U) || (scene->mRootNode == nullptr)) {
        throw std::runtime_error(importer.GetErrorString());
    }

    directory = path.substr(0, path.find_last_of('/'));

    this->boundingBox.min = AssimpGLMHelpers::getGLMVec(scene->mMeshes[0]->mAABB.mMin);
    this->boundingBox.max = AssimpGLMHelpers::getGLMVec(scene->mMeshes[0]->mAABB.mMax);

    processNode(scene->mRootNode, scene);
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

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex::Data vertex{};

        vertex.position = AssimpGLMHelpers::getGLMVec(mesh->mVertices[i]);

        if (mesh->HasNormals()) {
            vertex.normal = AssimpGLMHelpers::getGLMVec(mesh->mNormals[i]);
        }
        if (mesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
        {
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            const glm::vec3 vec = AssimpGLMHelpers::getGLMVec(mesh->mTextureCoords[0][i]);
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

    std::vector<Texture::Data> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE,
                                                                  Texture::Type::DIFFUSE);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture::Data> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR,
                                                                   Texture::Type::SPECULAR);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. height maps
    std::vector<Texture::Data> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT,
                                                                 Texture::Type::HEIGHT);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    // 4. normal maps
    std::vector<Texture::Data> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS,
                                                                 Texture::Type::NORMAL);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 5. ambient occlusion maps
    std::vector<Texture::Data> ambientOcclusionMaps = loadMaterialTextures(material, aiTextureType_AMBIENT,
                                                                           Texture::Type::AMBIENT_OCCLUSION);
    textures.insert(textures.end(), ambientOcclusionMaps.begin(), ambientOcclusionMaps.end());
    // 6. emissive maps
    std::vector<Texture::Data> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE,
                                                                   Texture::Type::EMISSIVE);
    textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    return {vertices, indices, textures};
}

auto Model::loadMaterialTextures(const aiMaterial *const mat, const aiTextureType type,
                                 const Texture::Type texType) -> std::vector<Texture::Data> {
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

        const Texture::Data texture{
            Texture::Loader::load(path, directory),
            texType,
            str.C_Str()
        };

        textures.push_back(texture);
        textures_loaded[path] = texture;
    }

    return textures;
}
