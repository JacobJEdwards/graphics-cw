//
// Created by Jacob Edwards on 02/04/2024.
//

#include "ProceduralTerrain.h"


#include "Clouds.h"
#include "utils/Noise.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <utility>
#include <vector>
#include "graphics/buffers/VertexBuffer.h"
#include "graphics/Vertex.h"
#include "graphics/Shader.h"
#include "utils/ShaderManager.h"
#include "utils/PlayerManager.h"
#include "utils/Random.h"

void ProceduralTerrain::Chunk::init() {
    buffer = std::make_unique<VertexBuffer>();
    buffer->fill(vertices, indices);
}

ProceduralTerrain::ProceduralTerrain(const glm::vec2 center, const int chunkSize,
                                     const int numChunksX,
                                     const int numChunksY)
    : centre(center), chunkSize(chunkSize), numChunksX(numChunksX), numChunksY(numChunksY) {
    shader = ShaderManager::GetInstance().get("Simple");
    chunks.reserve(static_cast<std::size_t>(numChunksX) * static_cast<std::size_t>(numChunksY));

    worldSizeX = static_cast<float>(chunkSize * numChunksX);
    worldSizeY = static_cast<float>(chunkSize * numChunksY);

    worldCentre = glm::vec2(worldSizeX / 2.0F, worldSizeY / 2.0F);

    generate();
}

void ProceduralTerrain::draw(const std::shared_ptr<Shader> shader) const {
    const auto player = PlayerManager::GetInstance().getCurrent();
    const glm::vec3 position = player->attributes.position;
    const int renderDistance = static_cast<int>(player->getCamera().getRenderDistance());

    const int xChunk = static_cast<int>(position.x + worldSizeX / 2.0F) / chunkSize;
    const int yChunk = static_cast<int>(position.z + worldSizeY / 2.0F) / chunkSize;

    const int startX = std::max(0, xChunk - renderDistance);
    const int endX = std::min(numChunksX, xChunk + renderDistance);

    const int startY = std::max(0, static_cast<int>(yChunk - renderDistance));
    const int endY = std::min(numChunksY, static_cast<int>(yChunk + renderDistance));

    shader->use();

    shader->setUniform("model", glm::mat4(1.0F));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, noiseTexture.id);
    shader->setUniform("noiseTexture", 1);

    for (int i = startY; i < endY; i++) {
        for (int j = startX; j < endX; j++) {
            const std::size_t index = i * numChunksX + j;
            const auto &chunk = chunks[index];

            chunk.buffer->bind();
            chunk.buffer->draw();
            chunk.buffer->unbind();
        }
    }
}

void ProceduralTerrain::draw(const glm::mat4 &view, const glm::mat4 &projection) const {
    shader->use();
    shader->setUniform("view", view);
    shader->setUniform("projection", projection);

    draw(shader);


    clouds.draw(view, projection);
    trees.draw(view, projection);
}

[[nodiscard]] auto ProceduralTerrain::getCentre() const -> glm::vec2 {
    return centre;
}

[[nodiscard]] auto ProceduralTerrain::getChunkSize() const -> int {
    return chunkSize;
}

[[nodiscard]] auto ProceduralTerrain::getNumChunksX() const -> int {
    return numChunksX;
}

[[nodiscard]] auto ProceduralTerrain::getNumChunksY() const -> int {
    return numChunksY;
}


[[nodiscard]] auto ProceduralTerrain::intersectRay(const glm::vec3 &rayStart) const -> bool {
    const float terrainHeight = getTerrainHeight(rayStart.x, rayStart.z);
    return rayStart.y <= terrainHeight;
}

[[nodiscard]] auto ProceduralTerrain::getWorldCoordinates(const glm::vec3 &position) const -> glm::vec2 {
    return {position.x + static_cast<float>(worldSizeX) / 2.0F, position.z + static_cast<float>(worldSizeY) / 2.0F};
}

[[nodiscard]] auto ProceduralTerrain::getWorldCoordinates(const glm::vec2 &position) const -> glm::vec2 {
    return getWorldCoordinates(glm::vec3(position.x, 0.0F, position.y));
}

[[nodiscard]] auto ProceduralTerrain::getWorldCoordinates(const float xPos, const float zPos) const -> glm::vec2 {
    return getWorldCoordinates(glm::vec2(xPos, zPos));
}

[[nodiscard]] constexpr auto ProceduralTerrain::getTerrainHeight(const glm::vec3 &position) const -> float {
    return getTerrainHeight(position.x, position.z);
}

[[nodiscard]] constexpr auto ProceduralTerrain::getTerrainHeight(const float xPos, const float zPos) const -> float {
    const float xCoord = xPos + static_cast<float>(worldSizeX) / 2.0F;
    const float zCoord = zPos + static_cast<float>(worldSizeY) / 2.0F;

    // cool mountains
    // return Noise::Simplex(glm::vec2(xCoord, zCoord), 0.1F, 8, 0.05F, 2.0F) * 100.0F;

    // flat terrain
    // return 0.0F;

    // hills
    return Noise::Simplex(glm::vec2(xCoord, zCoord), 0.1F, 8, 0.05F, 2.0F) * 10.0F;

    // fairly flat terrain
    // return Noise::Simplex(glm::vec2(xCoord, zCoord), 0.1F, 8, 0.2F, 2.0F);

    // hills with valleys
    // return Noise::Simplex(glm::vec2(xCoord, zCoord), 0.1F, 8, 0.2F, 2.0F) * 10.0F;

    // hills with valleys and mountains
    // return Noise::Simplex(glm::vec2(xCoord, zCoord), 0.1F, 8, 0.2F, 2.0F) * 10.0F + Noise::Simplex(glm::vec2(xCoord, zCoord), 0.1F, 8, 0.05F, 2.0F) * 100.0F;

    // extreme hills
    // return Noise::Simplex(glm::vec2(xCoord, zCoord), 0.1F, 8, 0.05F, 2.0F) * 100.0F + Noise::Simplex(glm::vec2(xCoord, zCoord), 0.1F, 8, 0.2F, 2.0F) * 10.0F;
}

[[nodiscard]] auto
ProceduralTerrain::getIntersectionPoint(const glm::vec3 &rayStart, const glm::vec3 &rayEnd) const -> glm::vec3 {
    const float terrainHeight = getTerrainHeight(rayStart.x, rayStart.z);
    const float distance = glm::distance(rayStart, rayEnd);
    const float t = (terrainHeight - rayStart.y) / distance;
    return rayStart + t * (rayEnd - rayStart);
}

[[nodiscard]] auto ProceduralTerrain::getTerrainNormal(const float x, const float y) const -> glm::vec3 {
    constexpr float dx = 0.1F;
    constexpr float dz = 0.1F;

    const float height1 = getTerrainHeight(x - dx, y);
    const float height2 = getTerrainHeight(x + dx, y);
    const float height3 = getTerrainHeight(x, y - dz);
    const float height4 = getTerrainHeight(x, y + dz);

    const glm::vec3 normal = glm::normalize(glm::vec3(height1 - height2, 2.0F * dx, height3 - height4));

    return normal;
}

[[nodiscard]] auto ProceduralTerrain::getTrees() const -> const Trees & {
    return trees;
}

[[nodiscard]] auto ProceduralTerrain::getClouds() const -> const Clouds & {
    return clouds;
}


constexpr void ProceduralTerrain::generate() {
    for (int y = 0; y < numChunksY; y++) {
        for (int x = 0; x < numChunksX; x++) {
            generateChunk(x, y);
        }
    }

    std::vector<glm::vec3> treePositions;
    for (int i = 0; i < NUM_TREE_INSTANCES; i++) {
        const float x = Random::Float(-worldSizeX / 2.0F, worldSizeX / 2.0F);
        const float z = Random::Float(-worldSizeY / 2.0F, worldSizeY / 2.0F);
        const float y = getTerrainHeight(x, z);

        // check if existing tree is too close
        bool tooClose = false;

        for (const auto &pos: treePositions) {
            if (glm::distance(glm::vec2(x, z), glm::vec2(pos.x, pos.z)) < 10.0F) {
                tooClose = true;
                break;
            }
        }

        if (tooClose) {
            continue;
        }

        if (glm::distance(getWorldCoordinates(glm::vec3(0.0F)), getWorldCoordinates(glm::vec3(x, y, z))) < 100.0F) {
            continue;
        }

        treePositions.emplace_back(x, y, z);
    }

    trees.generateTrees(treePositions);

    std::vector<glm::vec3> cloudPositions;
    for (int i = 0; i < NUM_CLOUD_INSTANCES; i++) {
        const float x = Random::Float(-worldSizeX / 2.0F, worldSizeX / 2.0F);
        const float z = Random::Float(-worldSizeY / 2.0F, worldSizeY / 2.0F);
        const float y = getTerrainHeight(x, z) + Random::Float(125.0F, 250.0F);

        cloudPositions.emplace_back(x, y, z);
    }

    clouds.generateClouds(cloudPositions);
}

constexpr void ProceduralTerrain::generateChunk(const int chunkX, const int chunkY) {
    const int xOffset = chunkX * chunkSize;
    const int yOffset = chunkY * chunkSize;

    Chunk chunk;

    chunk.centre = centre + glm::vec2(xOffset, yOffset);
    chunk.chunkSize = chunkSize;

    const auto worldCentre = glm::vec2(worldSizeX / 2.0F, worldSizeY / 2.0F);

    // generate vertices
    for (int i = 0; i < chunkSize + 1; i++) {
        for (int j = 0; j < chunkSize + 1; j++) {
            const float xCoord =
                    static_cast<float>(xOffset + j) - worldSizeX / 2.0F;
            const float zCoord =
                    static_cast<float>(yOffset + i) - worldSizeY / 2.0F;

            const float yCoord = getTerrainHeight(xCoord, zCoord);

            chunk.vertices.push_back(Vertex::Data{{xCoord, yCoord, zCoord}});
        }
    }

    // generate indices
    for (int i = 0; i < chunkSize; i++) {
        for (int j = 0; j < chunkSize; j++) {
            const int topLeft = j + i * (chunkSize + 1);
            const int topRight = j + 1 + i * (chunkSize + 1);
            const int bottomLeft = j + (i + 1) * (chunkSize + 1);
            const int bottomRight = j + 1 + (i + 1) * (chunkSize + 1);

            chunk.indices.push_back(topLeft);
            chunk.indices.push_back(bottomLeft);
            chunk.indices.push_back(topRight);

            chunk.indices.push_back(topRight);
            chunk.indices.push_back(bottomLeft);
            chunk.indices.push_back(bottomRight);
        }
    }

    for (std::size_t i = 0; i < chunk.indices.size(); i += 3) {
        const glm::vec3 v0 = chunk.vertices[chunk.indices[i]].position;
        const glm::vec3 v1 = chunk.vertices[chunk.indices[i + 1]].position;
        const glm::vec3 v2 = chunk.vertices[chunk.indices[i + 2]].position;

        const glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        chunk.vertices[chunk.indices[i]].normal += normal;
        chunk.vertices[chunk.indices[i + 1]].normal += normal;
        chunk.vertices[chunk.indices[i + 2]].normal += normal;
    }

    for (auto &vertex: chunk.vertices) {
        vertex.normal = glm::normalize(vertex.normal);
    }

    // texture coordinates
    for (auto &vertex: chunk.vertices) {
        vertex.texCoords = glm::vec2(vertex.position.x, vertex.position.z);
    }

    // tangent and bitangent
    for (std::size_t i = 0; i < chunk.indices.size(); i += 3) {
        const glm::vec3 v0 = chunk.vertices[chunk.indices[i]].position;
        const glm::vec3 v1 = chunk.vertices[chunk.indices[i + 1]].position;
        const glm::vec3 v2 = chunk.vertices[chunk.indices[i + 2]].position;

        const glm::vec2 uv0 = chunk.vertices[chunk.indices[i]].texCoords;
        const glm::vec2 uv1 = chunk.vertices[chunk.indices[i + 1]].texCoords;
        const glm::vec2 uv2 = chunk.vertices[chunk.indices[i + 2]].texCoords;

        const glm::vec3 deltaPos1 = v1 - v0;
        const glm::vec3 deltaPos2 = v2 - v0;

        const glm::vec2 deltaUV1 = uv1 - uv0;
        const glm::vec2 deltaUV2 = uv2 - uv0;

        const float f = 1.0F / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        const glm::vec3 tangent = f * (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y);
        const glm::vec3 bitangent = f * (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x);

        chunk.vertices[chunk.indices[i]].tangent += tangent;
        chunk.vertices[chunk.indices[i + 1]].tangent += tangent;
        chunk.vertices[chunk.indices[i + 2]].tangent += tangent;

        chunk.vertices[chunk.indices[i]].bitangent += bitangent;
        chunk.vertices[chunk.indices[i + 1]].bitangent += bitangent;
        chunk.vertices[chunk.indices[i + 2]].bitangent += bitangent;
    }

    chunk.init();
    chunks.push_back(std::move(chunk));
}


void ProceduralTerrain::generateGrass() {
    // use noise texture
    // vec4 noiseVal = texture(noiseTexture, fs_in.TexCoords);
    // lighting += vec3(noiseVal * 0.1);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    noiseTexture.id = texture;

    std::vector<unsigned char> data(4 * 512 * 512, 255);
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            // random noise
            data[4 * (512 * i + j) + 0] = Random::Int(0, 255);
            data[4 * (512 * i + j) + 1] = Random::Int(0, 255);
            data[4 * (512 * i + j) + 2] = Random::Int(0, 255);
            data[4 * (512 * i + j) + 3] = 255;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    // parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
