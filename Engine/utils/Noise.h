//
// Created by Jacob Edwards on 02/04/2024.
//

#ifndef CW_NOISE_H
#define CW_NOISE_H

#include <glm/gtc/noise.hpp>

namespace Noise {
    constexpr float SCALE = 0.1F;
    constexpr float AMPLITUDE = 1.0F;
    constexpr float FREQUENCY = 1.0F;
    constexpr unsigned int OCTAVES = 1U;
    constexpr float PERSISTENCE = 0.5F;
    constexpr float LACUNARITY = 2.0F;

    template<typename T>
    auto
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
    auto Simplex(const T &pos, const float scale = SCALE, const float amplitude = AMPLITUDE, const float frequency =
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
}

#endif //CW_NOISE_H
