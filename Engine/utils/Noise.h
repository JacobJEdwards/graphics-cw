//
// Created by Jacob Edwards on 02/04/2024.
//

#ifndef CW_NOISE_H
#define CW_NOISE_H

#include <glm/gtc/noise.hpp>

namespace Noise {
    namespace {
        constexpr float SCALE = 0.1F;
        constexpr float AMPLITUDE = 1.0F;
        constexpr float FREQUENCY = 1.0F;
        constexpr int OCTAVES = 1;
        constexpr float PERSISTENCE = 0.5F;
    }

    template<typename T>
    auto
    Perlin(const T &pos, float scale = SCALE, float amplitude = AMPLITUDE, float frequency = FREQUENCY,
           int octaves = OCTAVES,
           float persistence = PERSISTENCE) -> float {
        float noise = 0.0F;
        float maxNoise = 0.0F;
        float amp = amplitude;
        float freq = frequency;
        for (int i = 0; i < octaves; i++) {
            noise += glm::perlin(pos * scale * freq) * amp;
            maxNoise += amp;
            amp *= persistence;
            freq *= 2.0F;
        }
        return noise / maxNoise;
    }

    template<typename T>
    auto Simplex(const T &pos, float scale = SCALE, float amplitude = AMPLITUDE, float frequency = FREQUENCY,
                 int octaves = OCTAVES,
                 float persistence = PERSISTENCE) -> float {
        float noise = 0.0F;
        float maxNoise = 0.0F;
        float amp = amplitude;
        float freq = frequency;
        for (int i = 0; i < octaves; i++) {
            noise += glm::simplex(pos * scale * freq) * amp;
            maxNoise += amp;
            amp *= persistence;
            freq *= 2.0F;
        }
        return noise / maxNoise;
    }
}

#endif //CW_NOISE_H