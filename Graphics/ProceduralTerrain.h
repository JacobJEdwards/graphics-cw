//
// Created by Jacob Edwards on 24/02/2024.
//

#ifndef TERRAIN_H
#define TERRAIN_H

#include <cstddef>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Model.h"
#include "Perlin.h"
#include "stb_image.h"

constexpr int CHUNK_RENDER_DISTANCE = 3;
constexpr int X_MAP_CHUNKS = 10;
constexpr int Y_MAP_CHUNKS = 10;
constexpr int CHUNK_HEIGHT = 127;
constexpr int CHUNK_WIDTH = 127;

constexpr float WATER_HEIGHT = -0.5F;


class Terrain {
public:
    explicit Terrain(const int chunkRenderDistance = CHUNK_RENDER_DISTANCE, const int xMapChunks = X_MAP_CHUNKS,
                     const int yMapChunks = Y_MAP_CHUNKS,
                     const int chunkHeight = CHUNK_HEIGHT,
                     const int chunkWidth = CHUNK_WIDTH) : chunkRenderDistance(chunkRenderDistance),
                                                           xMapChunks(xMapChunks), yMapChunks(yMapChunks),
                                                           chunkHeight(chunkHeight), chunkWidth(chunkWidth),
                                                           originX(
                                                               static_cast<float>(chunkWidth * xMapChunks) / 2.0F -
                                                               static_cast<float>(chunkWidth) / 2.0F),
                                                           originY(
                                                               static_cast<float>(chunkHeight * yMapChunks) / 2.0F -
                                                               static_cast<float>(chunkHeight) / 2.0F),
                                                           mapChunks(
                                                               static_cast<std::vector<unsigned int>::size_type>(
                                                                   xMapChunks * yMapChunks)) {
        for (int y = 0; y < yMapChunks; y++) {
            for (int x = 0; x < xMapChunks; x++) {
                generateMapChunk(mapChunks[x + y * xMapChunks], x, y);
            }
        }

        shader.use();
        shader.setBool("isFlat", 1U);
        shader.setVec3("light.ambient", 0.2F, 0.2F, 0.2F);
        shader.setVec3("light.diffuse", 0.5F, 0.5F, 0.5F);
        shader.setVec3("light.specular", 1.0F, 1.0F, 1.0F);
        shader.setVec3("light.direction", -0.2F, -0.1F, -0.3F);
    }


    void draw(const glm::mat4 &view, const glm::mat4 &projection, const glm::vec3 &viewPos) const {
        shader.use();
        shader.setMat4("u_view", view);
        shader.setMat4("u_projection", projection);
        shader.setVec3("u_viewPos", viewPos);
        // Measures number of map chunks away from origin map chunk the camera is
        const int gridPosX = static_cast<int>((viewPos.x - originX) / chunkWidth + xMapChunks / 2);
        const int gridPosY = static_cast<int>((viewPos.z - originY) / chunkHeight + yMapChunks / 2);
        // Render map chunks
        for (int y = 0; y < yMapChunks; y++) {
            for (int x = 0; x < xMapChunks; x++) {
                // Only render chunk if it's within render distance
                const int distanceX = std::abs(gridPosX - x);
                const int distanceY = std::abs(gridPosY - y);

                if ((distanceX <= chunkRenderDistance) && (distanceY <= chunkRenderDistance) || (x < gridPosX || y < gridPosY)) {
                    auto model = glm::mat4(1.0F);
                    model = translate(model, glm::vec3(-chunkWidth / 2.0F + (chunkWidth - 1) * x, -2.0F,
                                                       -chunkHeight / 2.0F + (chunkHeight - 1.0F) * y));

                    shader.setMat4("u_model", model);

                    // Terrain chunk
                    glBindVertexArray(mapChunks[x + y * xMapChunks]);
                    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, nullptr);
               }
            }
        }
    }

    [[nodiscard]] auto getChunkWidth() const -> int {
        return chunkWidth;
    }

    [[nodiscard]] auto getRenderDistance() const -> int {
        return chunkRenderDistance;
    }

    [[nodiscard]] auto getOriginX() const -> float {
        return originX;
    }

    [[nodiscard]] auto getOriginY() const -> float {
        return originY;
    }

private:
    int chunkRenderDistance;
    int xMapChunks;
    int yMapChunks;
    int chunkHeight;
    int chunkWidth;

    float originX;
    float originY;

    std::vector<GLuint> mapChunks;

    int gridPositionX = 0;
    int gridPositionY = -10;

    int octaves = 5;
    float meshHeight = 2; // Vertical scaling
    float noiseScale = 30; // Horizontal scaling
    float persistence = 0.5;
    float lacunarity = 2;

    int nIndices = 0;

    Shader shader{"../Assets/shaders/terrain.vert", "../Assets/shaders/terrain.frag"};

    [[nodiscard]] auto generateNoiseMap(const int offsetX, const int offsetY) const -> std::vector<float> {
        std::vector<float> noiseValues;
        std::vector<float> normalizedNoiseValues;
        std::vector<int> p = getPermutationVector();

        float amp = 1;
        float freq = 1;
        float maxPossibleHeight = 0;

        for (int i = 0; i < octaves; i++) {
            maxPossibleHeight += amp;
            amp *= persistence;
        }

        for (int y = 0; y < chunkHeight; y++) {
            for (int x = 0; x < chunkWidth; x++) {
                amp = 1;
                freq = 1;
                float noiseHeight = 0;
                for (int i = 0; i < octaves; i++) {
                    const float xSample = static_cast<float>((x + offsetX * (chunkWidth - 1))) / noiseScale * freq;
                    const float ySample = static_cast<float>((y + offsetY * (chunkHeight - 1))) / noiseScale * freq;

                    const auto perlinValue = static_cast<float>(perlinNoise(xSample, ySample, p));
                    noiseHeight += perlinValue * amp;

                    // Lacunarity  --> Increase in frequency of octaves
                    // Persistence --> Decrease in amplitude of octaves
                    amp *= persistence;
                    freq *= lacunarity;
                }

                noiseValues.push_back(noiseHeight);
            }
        }

        for (int y = 0; y < chunkHeight; y++) {
            for (int x = 0; x < chunkWidth; x++) {
                // Inverse lerp and scale values to range from 0 to 1
                normalizedNoiseValues.push_back((noiseValues[x + y * chunkWidth] + 1) / maxPossibleHeight);
            }
        }

        return normalizedNoiseValues;
    }

    [[nodiscard]] auto generateVertices(const std::vector<float> &noise_map) const -> std::vector<float> {
        std::vector<float> v;

        for (int y = 0; y < chunkHeight + 1; y++) {
            for (int x = 0; x < chunkWidth; x++) {
                v.push_back(x);
                // Apply cubic easing to the noise
                const float easedNoise = static_cast<float>(std::pow(noise_map[x + y * chunkWidth] * 1.1, 3));
                // Scale noise to match meshHeight
                // Pervent vertex height from being below WATER_HEIGHT
                v.push_back(static_cast<float>(std::fmax(easedNoise * meshHeight, WATER_HEIGHT * 0.5 * meshHeight)));
                v.push_back(y);
            }
        }

        return v;
    }

    auto generateIndices() -> std::vector<int> {
        std::vector<int> indices;

        for (int y = 0; y < chunkHeight; y++) {
            for (int x = 0; x < chunkWidth; x++) {
                int pos = x + y * chunkWidth;

                if (x == chunkWidth - 1 || y == chunkHeight - 1) {
                    // Don't create indices for right or top edge
                    continue;
                } // Top left triangle of square
                indices.push_back(pos + chunkWidth);
                indices.push_back(pos);
                indices.push_back(pos + chunkWidth + 1);
                // Bottom right triangle of square
                indices.push_back(pos + 1);
                indices.push_back(pos + 1 + chunkWidth);
                indices.push_back(pos);
            }
        }

        nIndices = indices.size();

        return indices;
    }

    static auto generateNormals(const std::vector<int> &indices,
                                const std::vector<float> &vertices) -> std::vector<float> {
        std::vector<float> normals;
        std::vector<glm::vec3> verts;

        // Get the vertices of each triangle in mesh
        // For each group of indices
        for (int i = 0; i < indices.size(); i += 3) {
            // Get the vertices (point) for each index
            for (int j = 0; j < 3; j++) {
                const auto pos = indices[i + j] * 3;
                verts.emplace_back(vertices[pos], vertices[pos + 1], vertices[pos + 2]);
            }

            // Get vectors of two edges of triangle
            glm::vec3 U = verts[i + 1] - verts[i];
            glm::vec3 V = verts[i + 2] - verts[i];

            // Calculate normal
            auto normal = normalize(-cross(U, V));
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
        }

        return normals;
    }

    void generateMapChunk(GLuint &VAO, int x, int y) {
        const std::vector<int> indices = generateIndices();
        const std::vector<float> noiseMap = generateNoiseMap(x, y);
        const std::vector<float> vertices = generateVertices(noiseMap);
        const std::vector<float> normals = generateNormals(indices, vertices);
        const std::vector<float> colors = generateBiome(vertices, y);

        GLuint VBO[3];
        GLuint EBO;

        glGenBuffers(3, VBO);
        glGenBuffers(1, &EBO);
        glGenVertexArrays(1, &VAO);

        glBindVertexArray(VAO);

        // Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        // Normal buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);

        // Color buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(2);

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

        // Unbind VAO
        glBindVertexArray(0);
    }

    struct terrainColor {
        float height;
        glm::vec3 color;

        terrainColor(const float height, glm::vec3 color) : height(height), color(std::move(color)) {}
    };

    static auto getColor(int r, int g, int b) -> glm::vec3 {
        return {r / 255.0F, g / 255.0F, b / 255.0F};
    }

    static auto compareByHeight(const terrainColor& a, const terrainColor& b) -> bool {
        return a.height < b.height;
    }

    [[nodiscard]] static auto generateBiome(const std::vector<float>& vertices, int meshHeight) -> std::vector<float> {
        std::vector<float> colors;
        std::vector<terrainColor> biomeColors {
            {WATER_HEIGHT * 0.5F, getColor(60, 95, 190)}, // Deep water
            {WATER_HEIGHT, getColor(60, 100, 190)},       // Shallow water
            {0.15F, getColor(180, 200, 120)},             // Light green
            {0.30F, getColor(120, 170, 80)},              // Dark green
            {0.40F, getColor(90, 140, 50)},               // Darker green
            {0.50F, getColor(60, 100, 30)},               // Darkest green
            {0.80F, getColor(75, 60, 55)},                // Rock 2
            {1.00F, getColor(255, 255, 255)}              // Snow
        };

        // Sort biomeColors by height
        std::ranges::sort(biomeColors.begin(), biomeColors.end(), compareByHeight);

        // Iterate through vertex y values
        for (size_t i = 0; i < vertices.size(); i += 3) {
            const float vertexHeight = vertices[i + 1]; // Assuming y-coordinate is stored at index i+1
            glm::vec3 color = biomeColors.front().color; // Default color
            for (const auto& biomeColor : biomeColors) {
                if (vertexHeight <= biomeColor.height * meshHeight) {
                    color = biomeColor.color;
                    break;
                }
            }
            colors.push_back(color.r);
            colors.push_back(color.g);
            colors.push_back(color.b);
        }
        return colors;
    }
};


#endif //TERRAIN_H
