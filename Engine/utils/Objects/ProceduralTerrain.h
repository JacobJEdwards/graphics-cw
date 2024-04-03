//
// Created by Jacob Edwards on 02/04/2024.
//

#ifndef CW_PROCEDURALTERRAIN_H
#define CW_PROCEDURALTERRAIN_H

#include <iostream>
#include "utils/Noise.h"
#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <vector>
#include "utils/Buffer.h"
#include "utils/Vertex.h"
#include "utils/Shader.h"
#include <memory>
#include "graphics/Renderable.h"
#include "utils/ShaderManager.h"

constexpr auto DEFAULT_CENTRE = glm::vec2{0.0F, 0.0F};
constexpr auto DEFAULT_CHUNK_SIZE = 100;
constexpr auto DEFAULT_NUM_CHUNKS_X = 4;
constexpr auto DEFAULT_NUM_CHUNKS_Y = 4;


class ProceduralTerrain : public Renderable {
public:
    explicit ProceduralTerrain(glm::vec2 center = DEFAULT_CENTRE, int chunkSize = DEFAULT_CHUNK_SIZE,
                               int numChunksX = DEFAULT_NUM_CHUNKS_X,
                               int numChunksY = DEFAULT_NUM_CHUNKS_Y)
            : centre(center), chunkSize(chunkSize), numChunksX(numChunksX), numChunksY(numChunksY) {
        shader = ShaderManager::Get("Simple");
        buffers.resize(numChunksX * numChunksY);
        generate();
    }

    void draw(std::shared_ptr<Shader> shader) const override {
        for (const auto &buffer: buffers) {
            shader->use();
            buffer->bind();
            buffer->draw();
            buffer->unbind();
        }
    }

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override {
        for (const auto &buffer: buffers) {
            buffer->bind();
            shader->use();
            shader->setUniform("view", view);
            shader->setUniform("projection", projection);

            buffer->draw();

            buffer->unbind();
        }
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

    [[nodiscard]] auto getTerrainHeight(float x, float z) const -> float {
        const auto worldSizeX = static_cast<float>(chunkSize * numChunksX);
        const auto worldSizeY = static_cast<float>(chunkSize * numChunksY);

        const float xCoord = x + worldSizeX / 2.0F;
        const float zCoord = z + worldSizeY / 2.0F;

        return Noise::Simplex(glm::vec2(xCoord, zCoord));
    }

    [[nodiscard]] glm::vec3 getIntersectionPoint(const glm::vec3 &rayStart, const glm::vec3 &rayEnd) const {
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
    std::vector<std::unique_ptr<Buffer>> buffers;
    std::vector<std::vector<Vertex::Data>> chunkVertices;
    std::vector<std::vector<GLuint>> chunkIndices;

    // represents the centre of the terrain,, i.e. corner of 4 chunks
    glm::vec2 centre;
    int chunkSize;
    int numChunksX;
    int numChunksY;

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

        const auto worldSizeX = static_cast<float>(chunkSize * numChunksX);
        const auto worldSizeY = static_cast<float>(chunkSize * numChunksY);

        std::vector<Vertex::Data> vertices;
        std::vector<GLuint> indices;

        // generate vertices
        for (int i = 0; i < chunkSize + 1; i++) {
            for (int j = 0; j < chunkSize + 1; j++) {
                const float xCoord =
                        static_cast<float>(xOffset + j) - worldSizeX / 2.0F;
                const float zCoord =
                        static_cast<float>(yOffset + i) - worldSizeY / 2.0F;
                const float yCoord = Noise::Simplex(glm::vec2(xCoord, zCoord));
                vertices.push_back(Vertex::Data{{xCoord, yCoord, zCoord}});
            }
        }

        // generate indices
        for (int i = 0; i < chunkSize; i++) {
            for (int j = 0; j < chunkSize; j++) {
                const int topLeft = j + i * (chunkSize + 1);
                const int topRight = (j + 1) + i * (chunkSize + 1);
                const int bottomLeft = j + (i + 1) * (chunkSize + 1);
                const int bottomRight = (j + 1) + (i + 1) * (chunkSize + 1);

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        for (int i = 0; i < indices.size(); i += 3) {
            const glm::vec3 v0 = vertices[indices[i]].position;
            const glm::vec3 v1 = vertices[indices[i + 1]].position;
            const glm::vec3 v2 = vertices[indices[i + 2]].position;

            const glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            vertices[indices[i]].normal += normal;
            vertices[indices[i + 1]].normal += normal;
            vertices[indices[i + 2]].normal += normal;
        }

        for (auto &vertex: vertices) {
            vertex.normal = glm::normalize(vertex.normal);
        }

        chunkVertices.push_back(vertices);
        chunkIndices.push_back(indices);

        buffers[chunkX + chunkY * numChunksX] = std::make_unique<Buffer>();
        buffers[chunkX + chunkY * numChunksX]->fill(vertices, indices);
    }

    void generateNormals(std::vector<Vertex::Data> &vertices, const std::vector<GLuint> &indices) {
        for (int i = 0; i < indices.size(); i += 3) {
            const glm::vec3 v0 = vertices[indices[i]].position;
            const glm::vec3 v1 = vertices[indices[i + 1]].position;
            const glm::vec3 v2 = vertices[indices[i + 2]].position;

            const glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            vertices[indices[i]].normal += normal;
            vertices[indices[i + 1]].normal += normal;
            vertices[indices[i + 2]].normal += normal;
        }

        for (auto &vertex: vertices) {
            vertex.normal = glm::normalize(vertex.normal);
        }
    }

};

#endif //CW_PROCEDURALTERRAIN_H