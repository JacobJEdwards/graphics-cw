//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>
#include <iostream>

namespace Texture {
        void Loader::setWrapS(const GLint value) {
            wrapS = value;
        }

        void Loader::setWrapT(const GLint value) {
            wrapT = value;
        }

        void Loader::setMinFilter(const GLint value) {
            minFilter = value;
        }

        void Loader::setMagFilter(const GLint value) {
            magFilter = value;
        }

        auto Loader::getTexture() const -> GLuint {
            return texture;
        }

        void Loader::setFlip(const bool value) {
            stbi_set_flip_vertically_on_load(static_cast<int>(value));
        }

        void Loader::load(const std::string &path, const std::string &directory, bool gamma) {
            auto filename = std::string(path);
            filename = directory + '/' + filename;

            glGenTextures(1, &texture);

            int width;
            int height;
            int nrChannels;

            unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
            if (data != nullptr) {
                GLint format;

                switch (nrChannels) {
                    case 1:
                        format = GL_RED;
                        break;
                    case 2:
                        format = GL_RG;
                        break;
                    case 3:
                        format = GL_RGB;
                        break;
                    case 4:
                        format = GL_RGBA;
                        break;
                    default:
                        throw std::runtime_error("Error loading texture");
                }

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

    auto create(const std::string &path, const Type type) -> Data {
        Loader loader;

        loader.load(path, path, false);
        const GLuint textureId = loader.getTexture();
        return {textureId, type, path};
    }

    void del(const Data &texture) {
        glDeleteTextures(1, &texture.id);
    }
}