//
// Created by Jacob Edwards on 20/02/2024.
//

#ifndef CW_SHADER_H
#define CW_SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <string>

class Shader {
public:
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(std::filesystem::path vertexPath, std::filesystem::path fragmentPath,
           std::filesystem::path geometryPath = "", std::filesystem::path tessControlPath = "",
           std::filesystem::path tessEvalPath = "");

    ~Shader();

    auto operator=(Shader &&other) noexcept -> Shader &;

    Shader(Shader &&other) noexcept;

    Shader(const Shader &other) = delete;

    auto operator=(const Shader &other) -> Shader & = delete;

    [[nodiscard]] auto validateProgram() const -> bool;

    void use() const;

    void reload();

    [[nodiscard]] auto getProgramID() const -> GLuint;

    template<typename T>
    void setUniform(const std::string &name, T value) const;

private:
    std::filesystem::path vertexPath;
    std::filesystem::path fragmentPath;
    std::filesystem::path geometryPath;
    std::filesystem::path tessControlPath;
    std::filesystem::path tessEvalPath;

    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::string tessControlCode;
    std::string tessEvalCode;

    GLuint ID = 0;

    static constexpr int BUFFER_SIZE = 1024;

    static void checkCompileErrors(GLuint shader, bool isProgram);

    void deleteProgram() const;

    void load();

    static auto readShaderFile(const std::filesystem::path &path) -> std::string;

    static auto compileShader(const std::string &shaderCode, GLenum shaderType)
    -> GLuint;
};

#endif // CW_SHADER_H
