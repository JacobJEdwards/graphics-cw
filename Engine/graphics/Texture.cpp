//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION

#include "graphics/stb_image.h"

#include <GL/glew.h>
#include <filesystem>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>

namespace Texture {
    namespace Loader {
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
                std::cerr << "Failed to load texture: " << failureReason << std::endl;
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
                std::cerr << "Failed to load texture: " << failureReason << std::endl;
            }

            stbi_image_free(data);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            return texture;
        }

        auto loadCubemap(std::span<const std::string, CUBE_MAP_FACES> faces) -> GLuint {
            GLuint texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

            int width;
            int height;
            int nrChannels;

            for (unsigned int i = 0; i < faces.size(); i++) {
                stbi_uc *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
                if (data != nullptr) {
                    const GLint format = getFormat(nrChannels);

                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format,
                                 GL_UNSIGNED_BYTE, data);
                } else {
                    const char *failureReason = stbi_failure_reason();
                    std::cerr << "Failed to load texture: " << failureReason << std::endl;
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

        auto getFormat(const int nrChannels) -> GLint {
            switch (nrChannels) {
                case 1:
                    return GL_RED;
                case 2:
                    return GL_RG;
                case 3:
                    return GL_RGB;
                case 4:
                    return GL_RGBA;
                default:
                    throw std::runtime_error("Error loading texture");
            }
        }

        void setFlip(const bool flip) {
            stbi_set_flip_vertically_on_load(static_cast<int>(flip));
        }
    }

    auto toString(const Type type) -> std::string {
        switch (type) {
            case Type::DIFFUSE:
                return "diffuse";
            case Type::SPECULAR:
                return "specular";
            case Type::NORMAL:
                return "normal";
            case Type::HEIGHT:
                return "height";
            default:
                return "";
        }
    }

    void bind(const Data &texture) {
        glBindTexture(GL_TEXTURE_2D, texture.id);
    }

    void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void del(const Data &texture) {
        glDeleteTextures(1, &texture.id);
    }
}
