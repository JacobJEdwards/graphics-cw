//
// Created by Jacob Edwards on 02/04/2024.
//

#ifndef CW_PROCEDURALTERRAIN_H
#define CW_PROCEDURALTERRAIN_H

#include <algorithm>
#include "utils/Noise.h"
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <utility>
#include <vector>
#include "utils/Buffer.h"
#include "utils/Vertex.h"
#include "utils/Shader.h"
#include <memory>
#include "graphics/Renderable.h"
#include "utils/ShaderManager.h"
#include "utils/PlayerManager.h"

constexpr auto DEFAULT_CENTRE = glm::vec2{0.0F, 0.0F};
constexpr auto DEFAULT_CHUNK_SIZE = 100;
constexpr auto DEFAULT_NUM_CHUNKS_X = 16;
constexpr auto DEFAULT_NUM_CHUNKS_Y = 16;


class ProceduralTerrain : public Renderable {
    struct Chunk {
        std::unique_ptr<Buffer> buffer = nullptr;
        std::vector<Vertex::Data> vertices = {};
        std::vector<GLuint> indices = {};
        glm::vec2 centre = glm::vec2(0.0F, 0.0F);
        int chunkSize = DEFAULT_CHUNK_SIZE;

        void init() {
            buffer = std::make_unique<Buffer>();
            buffer->fill(vertices, indices);
        }
    };

public:
    explicit ProceduralTerrain(glm::vec2 center = DEFAULT_CENTRE, int chunkSize = DEFAULT_CHUNK_SIZE,
                               int numChunksX = DEFAULT_NUM_CHUNKS_X,
                               int numChunksY = DEFAULT_NUM_CHUNKS_Y)
            : centre(center), chunkSize(chunkSize), numChunksX(numChunksX), numChunksY(numChunksY) {
        shader = ShaderManager::Get("Simple");
        chunks.reserve(static_cast<std::size_t>(numChunksX) * static_cast<std::size_t>(numChunksY));

        worldSizeX = chunkSize * numChunksX;
        worldSizeY = chunkSize * numChunksY;
        generate();
    }

    void draw(std::shared_ptr<Shader> shader) const override {
        auto player = PlayerManager::GetCurrent();
        const glm::vec3 position = player->attributes.position;
        const int renderDistance = static_cast<int>(player->getCamera().getRenderDistance());

        const int xChunk = (position.x + worldSizeX / 2.0F) / chunkSize;
        const int yChunk = (position.z + worldSizeY / 2.0F) / chunkSize;

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

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override {
        shader->use();
        shader->setUniform("view", view);
        shader->setUniform("projection", projection);

        draw(shader);
    }

    [[nodiscard]] auto getCentre() const -> glm::vec2 {
        return centre;
    }

    [[nodiscard]] auto getChunkSize() const -> int {
        return chunkSize;
    }

    [[nodiscard]] auto getNumChunksX() const -> int {
        return numChunksX;
    }

    [[nodiscard]] auto getNumChunksY() const -> int {
        return numChunksY;
    }


    [[nodiscard]] auto intersectRay(const glm::vec3 &rayStart) const -> bool {
        const float terrainHeight = getTerrainHeight(rayStart.x, rayStart.z);
        return rayStart.y <= terrainHeight;
    }

    [[nodiscard]] auto getWorldCoordinates(const glm::vec3 &position) const -> glm::vec2 {
        return {position.x + static_cast<float>(worldSizeX) / 2.0F, position.z + static_cast<float>(worldSizeY) / 2.0F};
    }

    [[nodiscard]] auto getTerrainHeight(const glm::vec3 &position) const -> float {
        return getTerrainHeight(position.x, position.z);
    }

    [[nodiscard]] auto getTerrainHeight(float xPos, float zPos) const -> float {
        const float xCoord = xPos + static_cast<float>(worldSizeX) / 2.0F;
        const float zCoord = zPos + static_cast<float>(worldSizeY) / 2.0F;

        return Noise::Simplex(glm::vec2(xCoord, zCoord));
    }

    [[nodiscard]] auto getIntersectionPoint(const glm::vec3 &rayStart, const glm::vec3 &rayEnd) const -> glm::vec3 {
        const float terrainHeight = getTerrainHeight(rayStart.x, rayStart.z);
        const float distance = glm::distance(rayStart, rayEnd);
        const float t = (terrainHeight - rayStart.y) / distance;
        return rayStart + t * (rayEnd - rayStart);
    }

    // get teh normal of the terrain at a given point
    // can be used for collision detection
    [[nodiscard]] auto getTerrainNormal(float x, float y) const -> glm::vec3 {
        const float dx = 0.1F;
        const float dz = 0.1F;

        const float height1 = getTerrainHeight(x - dx, y);
        const float height2 = getTerrainHeight(x + dx, y);
        const float height3 = getTerrainHeight(x, y - dz);
        const float height4 = getTerrainHeight(x, y + dz);

        const glm::vec3 normal = glm::normalize(glm::vec3(height1 - height2, 2.0F * dx, height3 - height4));

        return normal;
    }


private:
    std::vector<Chunk> chunks;

    glm::vec2 centre;

    int chunkSize;

    int numChunksX;
    int numChunksY;

    int worldSizeX;
    int worldSizeY;

    void generate() {
        for (int y = 0; y < numChunksY; y++) {
            for (int x = 0; x < numChunksX; x++) {
                generateChunk(x, y);
            }
        }
    }

    void generateChunk(int chunkX, int chunkY) {
        const int xOffset = chunkX * chunkSize;
        const int yOffset = chunkY * chunkSize;

        Chunk chunk;

        chunk.centre = centre += glm::vec2(xOffset, yOffset);
        chunk.chunkSize = chunkSize;

        // generate vertices
        for (int i = 0; i < chunkSize + 1; i++) {
            for (int j = 0; j < chunkSize + 1; j++) {
                const float xCoord =
                        static_cast<float>(xOffset + j - worldSizeX) / 2.0F;
                const float zCoord =
                        static_cast<float>(yOffset + i - worldSizeY) / 2.0F;
                const float yCoord = Noise::Simplex(glm::vec2(xCoord, zCoord));

                chunk.vertices.push_back(Vertex::Data{{xCoord, yCoord, zCoord}});
            }
        }

        // generate indices
        for (int i = 0; i < chunkSize; i++) {
            for (int j = 0; j < chunkSize; j++) {
                const int topLeft = j + i * (chunkSize + 1);
                const int topRight = (j + 1) + i * (chunkSize + 1);
                const int bottomLeft = j + (i + 1) * (chunkSize + 1);
                const int bottomRight = (j + 1) + (i + 1) * (chunkSize + 1);

                chunk.indices.push_back(topLeft);
                chunk.indices.push_back(bottomLeft);
                chunk.indices.push_back(topRight);

                chunk.indices.push_back(topRight);
                chunk.indices.push_back(bottomLeft);
                chunk.indices.push_back(bottomRight);
            }
        }

        for (int i = 0; i < chunk.indices.size(); i += 3) {
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

        chunk.init();
        chunks.push_back(std::move(chunk));
    }
};

#endif //CW_PROCEDURALTERRAIN_H