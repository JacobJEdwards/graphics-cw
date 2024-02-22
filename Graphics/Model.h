//
// Created by Jacob Edwards on 22/02/2024.
//

#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <stack>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Texture.h"
#include "Vertex.h"
#include "Shader.h"
#include "../Utils/AssimpGLMHelpers.h"


class Model {
    using MeshPtr = std::unique_ptr<Mesh>;

public:
    explicit Model(const std::string &path, const bool gamma = false) : gammaCorrection(gamma) {
        loadModel(path);
    }

    void draw(const Shader &shader) const {
        for (const auto& mesh: meshes) {
            mesh->draw(shader);
        }
    }

private:
    Texture::Loader loader;

    std::vector<Texture::Data> textures_loaded;
    std::vector<MeshPtr> meshes;
    std::string directory;
    bool gammaCorrection;

    void loadModel(const std::string &path) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(
            path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if ((scene == nullptr) || ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0U) || (scene->mRootNode == nullptr)) {
            throw std::runtime_error(importer.GetErrorString());
        }

        directory = path.substr(0, path.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void processNode(const aiNode *const node, const aiScene *scene) {
        std::stack<const aiNode*> nodeStack;
        nodeStack.push(node);

        while (!nodeStack.empty()) {
            const aiNode *currentNode = nodeStack.top();
            nodeStack.pop();

            for (unsigned int i = 0; i < currentNode->mNumMeshes; ++i) {
                aiMesh *mesh = scene->mMeshes[currentNode->mMeshes[i]];
                meshes.push_back(std::make_unique<Mesh>(processMesh(mesh, scene)));
            }

            for (unsigned int i = 0; i < node->mNumChildren; i++) {
                nodeStack.push(currentNode->mChildren[i]);
            }
        }
    }

    auto processMesh(aiMesh *mesh, const aiScene *scene) -> Mesh {
        std::vector<Vertex::Data> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture::Data> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex::Data vertex{};

            vertex.position = AssimpGLMHelpers::getGlMVec(mesh->mVertices[i]);

            if (mesh->HasNormals()) {
                vertex.normal = AssimpGLMHelpers::getGlMVec(mesh->mNormals[i]);
            }
            if (mesh->mTextureCoords[0] != nullptr) // does the mesh contain texture coordinates?
            {
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                glm::vec3 vec = AssimpGLMHelpers::getGlMVec(mesh->mTextureCoords[0][i]);
                vertex.texCoords = glm::vec2(vec.x, vec.y);

                // tangent
                vertex.tangent = AssimpGLMHelpers::getGlMVec(mesh->mTangents[i]);
                // bitangent
                vertex.bitangent = AssimpGLMHelpers::getGlMVec(mesh->mBitangents[i]);
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
        // 3. normal maps
        std::vector<Texture::Data> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT,
                                                                     Texture::Type::SPECULAR);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture::Data> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT,
                                                                     Texture::Type::HEIGHT);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        return {vertices, indices, textures};
    }

    auto loadMaterialTextures(const aiMaterial * const mat, const aiTextureType type,
                              const Texture::Type texType) -> std::vector<Texture::Data> {
        std::vector<Texture::Data> textures;
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
            aiString str;
            mat->GetTexture(type, i, &str);

            for (unsigned int j = 0; j < textures_loaded.size(); j++) {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    break;
                }

                loader.load(str.C_Str(), this->directory, gammaCorrection);

                Texture::Data texture {
                    loader.getTexture(),
                    texType,
                    str.C_Str()
                };

                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }

        return textures;
    }

};


#endif //MODEL_H
