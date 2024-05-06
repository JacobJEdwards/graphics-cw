//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_TEXTURE_H
#define CW_TEXTURE_H

#include <GL/glew.h>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>

namespace Texture {
    namespace Loader {
        constexpr auto CUBE_MAP_FACES = 6U;

        auto load(const std::filesystem::path &path, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT,
                  GLint minFilter = GL_LINEAR_MIPMAP_LINEAR, GLint magFilter = GL_LINEAR) -> GLuint;

        auto load(const std::string &filename, const std::filesystem::path &directory, GLint wrapS = GL_REPEAT,
                  GLint wrapT = GL_REPEAT, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                  GLint magFilter = GL_LINEAR) -> GLuint;

        auto loadCubemap(const std::filesystem::path &path) -> GLuint;

        auto loadCubemap(std::span<const std::filesystem::path, CUBE_MAP_FACES> faces) -> GLuint;

        constexpr auto getFormat(const int nrChannels) -> GLint {
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
                    return GL_RGB;
            }
        }

        void setFlip(bool flip);
    }

    enum class Type {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT,
        CUBEMAP,
        AMBIENT_OCCLUSION,
        EMISSIVE,
    };

    constexpr auto toString(const Type type) -> std::string {
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

    struct Data {
        GLuint id = 0U;
        Type type = Type::DIFFUSE;
        std::string path;
        GLenum target = GL_TEXTURE_2D;
    };
}

#endif // CW_TEXTURE_H
