//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Texture.h"
#include <cstddef>
#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION

#include "graphics/stb_image.h"

#include <GL/glew.h>
#include <filesystem>
#include <iostream>
#include <print>
#include <span>
#include <string>

namespace Texture::Loader {
    auto
    load(const std::string &filename, const std::filesystem::path &directory, const GLint wrapS, const GLint wrapT,
         const GLint minFilter, const GLint magFilter) -> GLuint {
        return load(directory / filename, wrapS, wrapT, minFilter, magFilter);
    }

    auto load(const std::filesystem::path &path, const GLint wrapS, const GLint wrapT, const GLint minFilter,
              const GLint magFilter) -> GLuint {
        GLuint texture;
        glGenTextures(1, &texture);

        int width;
        int height;
        int nrChannels;

        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data != nullptr) {
            const GLint format = getFormat(nrChannels);

            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        } else {
            const char *failureReason = stbi_failure_reason();
            std::println(stderr, "Failed to load texture: {} - {}", path.c_str(), failureReason);
        }

        stbi_image_free(data);

        return texture;
    }

    auto loadCubemap(const std::filesystem::path &path) -> GLuint {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        int width;
        int height;
        int nrChannels;

        stbi_uc *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if (data != nullptr) {
            const GLint format = getFormat(nrChannels);

            for (unsigned int i = 0; i < CUBE_MAP_FACES; i++) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format,
                             GL_UNSIGNED_BYTE, data);
            }

            /*
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            */
        } else {
            const char *failureReason = stbi_failure_reason();
            std::println(stderr, "Failed to load texture: {} - {}", path.c_str(), failureReason);
        }

        stbi_image_free(data);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texture;
    }

    auto loadCubemap(const std::span<const std::filesystem::path, CUBE_MAP_FACES> faces) -> GLuint {
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        int width;
        int height;
        int nrChannels;

        for (std::size_t i = 0; i < faces.size(); i++) {
            stbi_uc *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data != nullptr) {
                const GLint format = getFormat(nrChannels);

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format,
                             GL_UNSIGNED_BYTE, data);
            } else {
                const char *failureReason = stbi_failure_reason();
                std::println(stderr, "Failed to load texture: {}", failureReason);
            }

            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return texture;
    }

    void setFlip(const bool flip) {
        stbi_set_flip_vertically_on_load(static_cast<int>(flip));
    }
}

