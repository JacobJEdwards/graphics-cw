//
// Created by Jacob Edwards on 21/02/2024.
//

#ifndef CW_TEXTURE_H
#define CW_TEXTURE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <string>
#include <iostream>

namespace Texture {
    class Loader {
    private:
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

        GLuint getTexture() const;

        void load(const std::string &path);

        static void setFlip(bool value);
    };


    enum class Type {
        DIFFUSE,
        SPECULAR,
        NORMAL,
        HEIGHT
    };

    std::string toString(Texture::Type type);

    struct Data {
        GLuint id;
        Texture::Type type;
        std::string path;
    };


    void bind(const Data &texture);

    void unbind();

    Texture::Data create(const std::string &path, Texture::Type type);

    void del(const Texture::Data &texture);
}


#endif //CW_TEXTURE_H
