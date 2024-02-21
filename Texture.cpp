//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <string>
#include <iostream>

namespace Texture {
        void Loader::setWrapS(GLint value) {
            wrapS = value;
        }

        void Loader::setWrapT(GLint value) {
            wrapT = value;
        }

        void Loader::setMinFilter(GLint value) {
            minFilter = value;
        }

        void Loader::setMagFilter(GLint value) {
            magFilter = value;
        }

        GLuint Loader::getTexture() const {
            return texture;
        }

        void Loader::setFlip(bool value) {
            stbi_set_flip_vertically_on_load(value);
        }

        void Loader::load(const std::string &path) {
            int width;
            int height;
            int nrChannels;

            unsigned char *data;

            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            // set the texture wrapping/filtering options (on the currently bound texture object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

            data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

            if (data) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else {
                const char *failureReason = stbi_failure_reason();
                std::cout << "Failed to load texture: " << failureReason << std::endl;
            }
            stbi_image_free(data);
        }

    std::string toString(Texture::Type type) {
        switch (type) {
            case Texture::Type::DIFFUSE:
                return "diffuse";
            case Texture::Type::SPECULAR:
                return "specular";
            case Texture::Type::NORMAL:
                return "normal";
            case Texture::Type::HEIGHT:
                return "height";
        }
    }

    void bind(const Texture::Data &texture) {
        glBindTexture(GL_TEXTURE_2D, texture.id);
    }

    void unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    Texture::Data create(const std::string &path, Texture::Type type) {
        Texture::Loader loader;

        loader.load(path);
        GLuint id = loader.getTexture();
        return {id, type, path};
    }

    void del(const Texture::Data &texture) {
        glDeleteTextures(1, &texture.id);
    }
}