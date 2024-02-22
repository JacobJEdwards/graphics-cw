//
// Created by Jacob Edwards on 20/02/2024.
//

#ifndef CW_SHADER_H
#define CW_SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

class Shader {
public:
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(std::string vertexPath, std::string fragmentPath,
           std::string geometryPath = "", std::string tessControlPath = "", std::string tessEvalPath = "");

    ~Shader();

    auto operator=(Shader &&other) noexcept -> Shader &;

    Shader(Shader &&other) noexcept;

    Shader(const Shader &) = delete;

    auto operator=(const Shader &) -> Shader & = delete;

    [[nodiscard]] auto validateProgram() const -> bool;

    void use() const;

    void reload();

    [[nodiscard]] auto getProgramID() const -> GLuint;

    void setBool(const std::string &name, GLboolean value) const;

    void setInt(const std::string &name, GLint value) const;

    void setFloat(const std::string &name, GLfloat value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setVec2(const std::string &name, float x, float y) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec3(const std::string &name, float x, float y, float z) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec4(const std::string &name, float x, float y, float z, float w) const;

    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    std::string vertexPath;
    std::string fragmentPath;
    std::string geometryPath;
    std::string tessControlPath;
    std::string tessEvalPath;

    GLuint ID = 0;

    static constexpr int BUFFER_SIZE = 1024;

    static void checkCompileErrors(GLuint shader, const std::string &type);

    void deleteProgram() const;

    void load();
};

#endif //CW_SHADER_H
