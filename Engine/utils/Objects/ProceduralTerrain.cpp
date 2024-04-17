//
// Created by Jacob Edwards on 02/04/2024.
//

#include "ProceduralTerrain.h"


#include "utils/Noise.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <utility>
#include "utils/Buffer.h"
#include "utils/Vertex.h"
#include "utils/Shader.h"
#include "utils/ShaderManager.h"
#include "utils/PlayerManager.h"

void ProceduralTerrain::Chunk::init() {
    buffer = std::make_unique<Buffer>();
    buffer->fill(vertices, indices);
}

ProceduralTerrain::ProceduralTerrain(const glm::vec2 center, const int chunkSize,
                                     const int numChunksX,
                                     const int numChunksY)
    : centre(center), chunkSize(chunkSize), numChunksX(numChunksX), numChunksY(numChunksY) {
    shader = ShaderManager::Get("Simple");
    chunks.reserve(static_cast<std::size_t>(numChunksX) * static_cast<std::size_t>(numChunksY));

    worldSizeX = static_cast<float>(chunkSize * numChunksX);
    worldSizeY = static_cast<float>(chunkSize * numChunksY);

    worldCentre = glm::vec2(worldSizeX / 2.0F, worldSizeY / 2.0F);

    generate();
}

void ProceduralTerrain::draw(const std::shared_ptr<Shader> shader) const {
    const auto player = PlayerManager::GetCurrent();
    const glm::vec3 position = player->attributes.position;
    const int renderDistance = static_cast<int>(player->getCamera().getRenderDistance());

    const int xChunk = static_cast<int>(position.x + worldSizeX / 2.0F) / chunkSize;
    const int yChunk = static_cast<int>(position.z + worldSizeY / 2.0F) / chunkSize;

    const int startX = std::max(0, xChunk - renderDistance);
    const int endX = std::min(numChunksX, xChunk + renderDistance);

    const int startY = std::max(0, static_cast<int>(yChunk - renderDistance));
    const int endY = std::min(numChunksY, static_cast<int>(yChunk + renderDistance));

    shader->use();

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

[[nodiscard]] auto ProceduralTerrain::getTerrainHeight(const glm::vec3 &position) const -> float {
    return getTerrainHeight(position.x, position.z);
}

[[nodiscard]] auto ProceduralTerrain::getTerrainHeight(const float xPos, const float zPos) const -> float {
    const float xCoord = xPos + static_cast<float>(worldSizeX) / 2.0F;
    const float zCoord = zPos + static_cast<float>(worldSizeY) / 2.0F;

    return Noise::Simplex(glm::vec2(xCoord, zCoord));
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


void ProceduralTerrain::generate() {
    for (int y = 0; y < numChunksY; y++) {
        for (int x = 0; x < numChunksX; x++) {
            generateChunk(x, y);
        }
    }
}

void ProceduralTerrain::generateChunk(const int chunkX, const int chunkY) {
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

            const float yCoord = Noise::Simplex(glm::vec2(xCoord, zCoord));

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
