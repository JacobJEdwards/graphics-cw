//
// Created by Jacob Edwards on 02/04/2024.
//

#ifndef CW_PROCEDURALTERRAIN_H
#define CW_PROCEDURALTERRAIN_H

#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>
#include "graphics/buffers/VertexBuffer.h"
#include "graphics/Vertex.h"
#include "graphics/Shader.h"
#include <memory>

#include "renderables/objects/Trees.h"
#include "renderables/Renderable.h"

constexpr auto DEFAULT_CENTRE = glm::vec2{0.0F, 0.0F};
constexpr auto DEFAULT_CHUNK_SIZE = 64;
constexpr auto DEFAULT_NUM_CHUNKS_X = 16;
constexpr auto DEFAULT_NUM_CHUNKS_Y = 16;
constexpr auto NUM_TREE_INSTANCES = 200;

class ProceduralTerrain final : public Renderable {
    struct Chunk {
        std::unique_ptr<VertexBuffer> buffer = nullptr;
        std::vector<Vertex::Data> vertices;
        std::vector<GLuint> indices;
        glm::vec2 centre = glm::vec2(0.0F, 0.0F);
        int chunkSize = DEFAULT_CHUNK_SIZE;

        void init();
    };

public:
    explicit ProceduralTerrain(glm::vec2 center = DEFAULT_CENTRE, int chunkSize = DEFAULT_CHUNK_SIZE,
                               int numChunksX = DEFAULT_NUM_CHUNKS_X,
                               int numChunksY = DEFAULT_NUM_CHUNKS_Y);

    void draw(std::shared_ptr<Shader> shader) const override;

    void draw(const glm::mat4 &view, const glm::mat4 &projection) const override;

    [[nodiscard]] auto getCentre() const -> glm::vec2;

    [[nodiscard]] auto getChunkSize() const -> int;

    [[nodiscard]] auto getNumChunksX() const -> int;

    [[nodiscard]] auto getNumChunksY() const -> int;


    [[nodiscard]] auto intersectRay(const glm::vec3 &rayStart) const -> bool;

    [[nodiscard]] auto getWorldCoordinates(const glm::vec3 &position) const -> glm::vec2;

    [[nodiscard]] auto getWorldCoordinates(float xPos, float zPos) const -> glm::vec2;

    [[nodiscard]] auto getWorldCoordinates(const glm::vec2 &position) const -> glm::vec2;

    [[nodiscard]] auto getTerrainHeight(const glm::vec3 &position) const -> float;

    [[nodiscard]] auto getTerrainHeight(float xPos, float zPos) const -> float;

    [[nodiscard]] auto getIntersectionPoint(const glm::vec3 &rayStart, const glm::vec3 &rayEnd) const -> glm::vec3;

    [[nodiscard]] auto getTerrainNormal(float x, float y) const -> glm::vec3;

    [[nodiscard]] auto getTrees() const -> const Trees &;

private:
    std::vector<Chunk> chunks;
    std::vector<Chunk> distantChunks;

    glm::vec2 centre;
    glm::vec2 worldCentre;

    // grass
    Texture::Data noiseTexture;

    int chunkSize;
    int numChunksX;
    int numChunksY;

    float worldSizeX;
    float worldSizeY;

    void generate();

    void generateChunk(int chunkX, int chunkY);

    Trees trees;

    void generateGrass();
};

#endif //CW_PROCEDURALTERRAIN_H
