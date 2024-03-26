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
    public:
        GLuint id = 0;
        Type type;
        GLenum nativeType = GL_TEXTURE_2D;

        std::string path;

        explicit Data(Type type = Type::DIFFUSE, const std::string &path = "");

        ~Data();

        void bind() const;

        void unbind() const;

    private:
        void setup();
    };

    namespace Loader {
        constexpr auto CUBE_MAP_FACES = 6;

        auto load(const std::filesystem::path &path, GLint wrapS = GL_REPEAT, GLint wrapT = GL_REPEAT,
                  GLint minFilter = GL_LINEAR_MIPMAP_LINEAR, GLint magFilter = GL_LINEAR) -> Data;

        auto load(const std::string &filename, const std::filesystem::path &directory, GLint wrapS = GL_REPEAT,
                  GLint wrapT = GL_REPEAT, GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
                  GLint magFilter = GL_LINEAR) -> Data;

        auto loadCubemap(const std::filesystem::path &path) -> Data;

        auto loadCubemap(std::span<const std::string, CUBE_MAP_FACES> faces) -> Data;

        auto getFormat(int nrChannels) -> GLint;

        void setFlip(bool flip);
    }
}

#endif // CW_TEXTURE_H
