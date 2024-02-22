//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_TEXTURE_H
#define CW_TEXTURE_H

#include <GL/glew.h>
#include <string>

namespace Texture {
    class Loader {
        GLuint texture = 0;
        GLint wrapS = GL_REPEAT;
        GLint wrapT = GL_REPEAT;
        GLint minFilter = GL_LINEAR;
        GLint magFilter = GL_LINEAR;

    public:
        Loader() = default;

        void setWrapS(GLint value);
        void setWrapT(GLint value);
        void setMinFilter(GLint value);
        void setMagFilter(GLint value);

        auto getTexture() const -> GLuint;

        void load(const std::string &path, const std::string &directory, bool gamma);

        static void setFlip(bool value);
    };


    enum class Type {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT
    };

    auto toString(Type type) -> std::string;

    struct Data {
        GLuint id;
        Type type;
        std::string path;
    };


    void bind(const Data &texture);

    void unbind();

    auto create(const std::string &path, Type type) -> Data;

    void del(const Data &texture);
}


#endif //CW_TEXTURE_H
