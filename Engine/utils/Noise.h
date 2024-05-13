//
// Created by Jacob Edwards on 02/04/2024.
//
/*
 *https://en.wikipedia.org/wiki/Perlin_noise
 *https://en.wikipedia.org/wiki/Simplex_noise
 */

#ifndef CW_NOISE_H
#define CW_NOISE_H

#include <glm/gtc/noise.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/glm.hpp>

namespace Noise {
    constexpr float SCALE = 0.1F;
    constexpr float AMPLITUDE = 1.0F;
    constexpr float FREQUENCY = 1.0F;
    constexpr unsigned int OCTAVES = 1U;
    constexpr float PERSISTENCE = 0.5F;
    constexpr float LACUNARITY = 2.0F;

    template<typename T>
    constexpr auto
    Perlin(const T &pos, const float scale = SCALE, const float amplitude = AMPLITUDE,
           const float frequency = FREQUENCY,
           const int octaves = OCTAVES,
           const float persistence = PERSISTENCE, const float lacunarity = LACUNARITY) -> float {
        float noise = 0.0F;
        float maxNoise = 0.0F;
        float amp = amplitude;
        float freq = frequency;
        for (int i = 0; i < octaves; i++) {
            noise += glm::perlin(pos * scale * freq) * amp;
            maxNoise += amp;
            amp *= persistence;
            freq *= lacunarity;
        }
        return noise / maxNoise;
    }

    template<typename T>
    constexpr auto Simplex(const T &pos, const float scale = SCALE, const float amplitude = AMPLITUDE, const float
                           frequency =
                                   FREQUENCY,
                           const int octaves = OCTAVES,
                           const float persistence = PERSISTENCE, const float lacunarity = LACUNARITY) -> float {
        float noise = 0.0F;
        float maxNoise = 0.0F;
        float amp = amplitude;
        float freq = frequency;
        for (int i = 0; i < octaves; i++) {
            noise += glm::simplex(pos * scale * freq) * amp;
            maxNoise += amp;
            amp *= persistence;
            freq *= lacunarity;
        }
        return noise / maxNoise;
    }

    inline auto rand(const glm::vec2 &co) -> float {
        return glm::fract(glm::sin(glm::dot(co, glm::vec2(12.9898, 78.233))) * 43758.5453);
    }

    inline auto noise(const glm::vec2 &p) -> float {
        const glm::vec2 ip = glm::floor(p);
        glm::vec2 u = glm::fract(p);
        u = u * u * (3.0F - 2.0F * u);

        const float res = glm::mix(
            glm::mix(rand(ip), rand(ip + glm::vec2(1.0F, 0.0F)), u.x),
            glm::mix(rand(ip + glm::vec2(0.0F, 1.0F)), rand(ip + glm::vec2(1.0F, 1.0F)), u.x),
            u.y
        );
        return res * res;
    }

    inline auto FBM(const glm::vec2 &p) -> float {
        float f = 0.0F;
        f += 0.5000F * noise(p);
        f += 0.2500F * noise(p * 2.02F);
        f += 0.1250F * noise(p * 2.03F);
        f += 0.0625F * noise(p * 2.01F);
        return f / 0.9375F;
    }
}

#endif //CW_NOISE_H
