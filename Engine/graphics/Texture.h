//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_TEXTURE_H
#define CW_TEXTURE_H

#include <GL/glew.h>
#include <filesystem>
#include <span>
#include <string>

namespace Texture {
    namespace Loader {
        constexpr auto CUBE_MAP_FACES = 6;

        auto load(const std::filesystem::path &path, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT,
                  GLint minFilter = GL_LINEAR_MIPMAP_LINEAR, GLint magFilter = GL_LINEAR) -> GLuint;

        auto load(const std::string &filename, const std::filesystem::path &directory, GLint wrapS = GL_REPEAT,
                  GLint wrapT = GL_REPEAT, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                  GLint magFilter = GL_LINEAR) -> GLuint;

        auto loadCubemap(const std::filesystem::path &path) -> GLuint;

        auto loadCubemap(std::span<const std::string, CUBE_MAP_FACES> faces) -> GLuint;

        auto getFormat(int nrChannels) -> GLint;

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

    auto toString(Type type) -> std::string;

    struct Data {
        GLuint id = 0;
        Type type = Type::DIFFUSE;
        std::string path = "";
        GLenum target = GL_TEXTURE_2D;

        /*
        Data();

        Data(GLuint id, Type type, std::string path);

        Data(const Data &other);

        Data(Data &&other) noexcept;

        auto operator=(const Data &other) -> Data &;

        auto operator=(Data &&other) noexcept -> Data &;

        ~Data();

        void init();

        void bind() const;

        void unbind() const;
         */
    };

    void bind(const Data &texture);

    void unbind();

    void del(const Data &texture);
}

#endif // CW_TEXTURE_H
