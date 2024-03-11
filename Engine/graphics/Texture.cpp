//
// Created by Jacob Edwards on 21/02/2024.
//

#include "Texture.h"

#include <stdexcept>
#include <string>
#include <array>
#include <GL/glew.h>
#include <iostream>
#include "SOIL2/SOIL2.h"
#include <filesystem>
#include <vector>

namespace Texture {
    namespace Loader {
        unsigned int flipImage = 0;

        auto load(const std::string &filename, const std::filesystem::path &directory, const bool gamma,
                  const GLint wrapS,
                  const GLint wrapT, const GLint minFilter, const GLint magFilter) -> GLuint {
            return load(directory / filename, gamma, wrapS, wrapT, minFilter, magFilter);
        }

        auto load(const std::filesystem::path &path, const bool /*gamma*/, const GLint wrapS, const GLint wrapT,
                  const GLint minFilter, const GLint magFilter) -> GLuint {
            const GLuint texture = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                                                   SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB |
                                                   SOIL_FLAG_COMPRESS_TO_DXT | flipImage);

            if (texture == 0) {
                std::cerr << "Failed to load texture: " << SOIL_last_result() << std::endl;
                throw std::runtime_error("Failed to load texture");
            }

            return texture;
        }

        auto loadCubemap(const std::filesystem::path &path) -> GLuint {
            const GLuint texture = SOIL_load_OGL_single_cubemap(path.c_str(), SOIL_DDS_CUBEMAP_FACE_ORDER, SOIL_LOAD_AUTO,
                                                          SOIL_CREATE_NEW_ID,
                                                          SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB |
                                                          SOIL_FLAG_COMPRESS_TO_DXT);

            if (texture == 0) {
                std::cerr << "Failed to load cubemap: " << SOIL_last_result() << std::endl;
                throw std::runtime_error("Failed to load cubemap");
            }

            return texture;
        }

        auto loadCubemap(const std::vector<std::string> &faces) -> GLuint {
            if (faces.size() != CUBE_MAP_FACES) {
                throw std::runtime_error("Invalid number of faces for cubemap");
            }
            // convert to array and call other function
            return loadCubemap(std::array {
                faces[0], faces[1], faces[2], faces[3], faces[4], faces[5]
            });
        }


        auto loadCubemap(const std::array<std::string, CUBE_MAP_FACES> &faces) -> GLuint {
            if (faces.size() != CUBE_MAP_FACES) {
                throw std::runtime_error("Invalid number of faces for cubemap");
            }
            const GLuint texture = SOIL_load_OGL_cubemap(faces[0].c_str(), faces[1].c_str(), faces[2].c_str(),
                                                   faces[3].c_str(), faces[4].c_str(), faces[5].c_str(), SOIL_LOAD_RGB,
                                                   SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

            if (texture == 0) {
                std::cerr << "Failed to load cubemap: " << SOIL_last_result() << std::endl;
                throw std::runtime_error("Failed to load cubemap");
            }

            return texture;
        }

        void setFlip(const bool flip) {
            if (flip) {
                flipImage = SOIL_FLAG_INVERT_Y;
            } else {
                flipImage = 0;
            }
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
