//
// Created by Jacob Edwards on 20/02/2024.
//

#include <array>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <glm/fwd.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/Shader.h"

Shader::Shader(std::filesystem::path vertexPath, std::filesystem::path fragmentPath,
               std::filesystem::path geometryPath, std::filesystem::path tessControlPath,
               std::filesystem::path tessEvalPath)
    : vertexPath(std::move(vertexPath)), fragmentPath(std::move(fragmentPath)),
      geometryPath(std::move(geometryPath)),
      tessControlPath(std::move(tessControlPath)),
      tessEvalPath(std::move(tessEvalPath)) {
    load();
}

Shader::~Shader() { deleteProgram(); }

auto Shader::operator=(Shader &&other) noexcept -> Shader & {
    if (this != &other) {
        ID = other.ID;
        other.ID = 0;
    }
    return *this;
}

Shader::Shader(Shader &&other) noexcept: ID(other.ID) { other.ID = 0; }

[[nodiscard]] auto Shader::validateProgram() const -> bool {
    glValidateProgram(ID);
    GLint success;
    glGetProgramiv(ID, GL_VALIDATE_STATUS, &success);
    if (success == 0) {
        GLint infoLogLength;
        glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<GLchar> infoLog(infoLogLength);
        glGetProgramInfoLog(ID, infoLogLength, nullptr, infoLog.data());
        std::cerr << "ERROR::SHADER::PROGRAM_VALIDATION_FAILED\n"
                << infoLog.data() << std::endl;
        std::cerr << vertexPath << "\n" << fragmentPath << std::endl;
        return false;
    }
    return true;
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::reload() {
    deleteProgram();
    load();
}

[[nodiscard]] auto Shader::getProgramID() const -> GLuint { return ID; }

template<typename T>
void Shader::setUniform(const std::string &name, T /*value*/) const {
    const std::string error = "Invalid type for uniform " + name;
    std::cerr << error << std::endl;
}

template<>
void Shader::setUniform(const std::string &name, const glm::mat4 value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const glm::mat3 value) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const glm::vec4 value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const glm::vec3 value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const glm::vec2 value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

template<>
void Shader::setUniform(const std::string &name, const int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

template<>
void Shader::setUniform(const std::string &name,
                        const unsigned int value) const {
    glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}

template<>
void Shader::setUniform(const std::string &name, const bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()),
                static_cast<GLint>(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::ivec2 &value) const {
    glUniform2iv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::ivec3 &value) const {
    glUniform3iv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::ivec4 &value) const {
    glUniform4iv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::uvec2 &value) const {
    glUniform2uiv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::uvec3 &value) const {
    glUniform3uiv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::uvec4 &value) const {
    glUniform4uiv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const glm::mat2 &value) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::mat2x3 &value) const {
    glUniformMatrix2x3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                         value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::mat2x4 &value) const {
    glUniformMatrix2x4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                         value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::mat3x2 &value) const {
    glUniformMatrix3x2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                         value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::mat3x4 &value) const {
    glUniformMatrix3x4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                         value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::mat4x2 &value) const {
    glUniformMatrix4x2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                         value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::mat4x3 &value) const {
    glUniformMatrix4x3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                         value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::dmat2 &value) const {
    glUniformMatrix2dv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::dmat3 &value) const {
    glUniformMatrix3dv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::dmat4 &value) const {
    glUniformMatrix4dv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                       value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::dmat2x3 &value) const {
    glUniformMatrix2x3dv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                         value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name,
                        const glm::dmat2x4 &value) const {
    glUniformMatrix2x4dv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE,
                         value_ptr(value));
}


template<typename T>
void Shader::setUniform(const std::string &name, T /*value*/, std::size_t /*count*/) const {
    const std::string error = "Invalid type for uniform " + name;
    std::cerr << error << std::endl;
}

template<>
void Shader::setUniform(const std::string &name, const glm::mat4 value, const std::size_t count) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE,
                       value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const glm::mat3 value, const std::size_t count) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), count, GL_FALSE,
                       value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const glm::vec4 value, const std::size_t count) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), count, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const glm::vec3 value, const std::size_t count) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), count, value_ptr(value));
}

template<>
void Shader::setUniform(const std::string &name, const glm::vec2 value, const std::size_t count) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), count, value_ptr(value));
}


template<typename T>
auto Shader::getUniform(const std::string &name) const -> T {
    const std::string error = "Invalid type for uniform " + name;
    std::cerr << error << std::endl;

    return nullptr;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::mat4 {
    glm::mat4 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::mat3 {
    glm::mat3 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::vec4 {
    glm::vec4 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::vec3 {
    glm::vec3 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::vec2 {
    glm::vec2 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> float {
    float value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), &value);
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> int {
    int value;
    glGetUniformiv(ID, glGetUniformLocation(ID, name.c_str()), &value);
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> unsigned int {
    unsigned int value;
    glGetUniformuiv(ID, glGetUniformLocation(ID, name.c_str()), &value);
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> bool {
    int value;
    glGetUniformiv(ID, glGetUniformLocation(ID, name.c_str()), &value);
    return static_cast<bool>(value);
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::ivec2 {
    glm::ivec2 value;
    glGetUniformiv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::ivec3 {
    glm::ivec3 value;
    glGetUniformiv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::ivec4 {
    glm::ivec4 value;
    glGetUniformiv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::uvec2 {
    glm::uvec2 value;
    glGetUniformuiv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::uvec3 {
    glm::uvec3 value;
    glGetUniformuiv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::uvec4 {
    glm::uvec4 value;
    glGetUniformuiv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::mat2 {
    glm::mat2 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::mat2x3 {
    glm::mat2x3 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::mat2x4 {
    glm::mat2x4 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::mat3x2 {
    glm::mat3x2 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::mat3x4 {
    glm::mat3x4 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::mat4x2 {
    glm::mat4x2 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::mat4x3 {
    glm::mat4x3 value;
    glGetUniformfv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::dmat2 {
    glm::dmat2 value;
    glGetUniformdv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}

template<>
auto Shader::getUniform(const std::string &name) const -> glm::dmat3 {
    glm::dmat3 value;
    glGetUniformdv(ID, glGetUniformLocation(ID, name.c_str()), value_ptr(value));
    return value;
}


void Shader::checkCompileErrors(const GLuint shader, const bool isProgram) {
    GLint success;
    std::array<GLchar, BUFFER_SIZE> infoLog{};

    if (!isProgram) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == 0) {
            glGetShaderInfoLog(shader, BUFFER_SIZE, nullptr, infoLog.data());
            std::cerr << "Shader compilation error" << "\n" << infoLog.data()
                    << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (success == 0) {
            glGetProgramInfoLog(shader, BUFFER_SIZE, nullptr, infoLog.data());
            std::cerr << "Program linking error" << "\n" << infoLog.data()
                    << std::endl;
        }
    }
}

void Shader::deleteProgram() const {
    if (ID != 0) {
        glDeleteProgram(ID);
        if (const GLuint error = glGetError(); error != GL_NO_ERROR) {
            std::cerr << "OpenGL error after deleting program: " << error
                    << std::endl;
        }
    }
}


// read shader file and process includes
auto Shader::readShaderFile(const std::filesystem::path &path) -> std::string {
    std::ifstream shaderFile(path);

    if (!shaderFile.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }

    // process includes
    std::string shaderCode;
    std::string line;
    while (std::getline(shaderFile, line)) {
        if (line.find("#include") != std::string::npos) {
            const std::string includePath =
                    line.substr(line.find_first_of('\"') + 1,
                                line.find_last_of('\"') - line.find_first_of('\"') - 1);
            shaderCode += readShaderFile(path.parent_path() / includePath);
        } else {
            shaderCode += line + "\n";
        }
    }

    return shaderCode;
}

auto Shader::compileShader(const std::string &shaderCode, const GLenum shaderType)
    -> GLuint {
    const char *code = shaderCode.c_str();
    const GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader, false);
    return shader;
}

void Shader::load() {
    try {
        if (vertexCode.empty()) {
            vertexCode = readShaderFile(vertexPath);
        }

        if (fragmentCode.empty()) {
            fragmentCode = readShaderFile(fragmentPath);
        }

        if (!geometryPath.empty() && geometryCode.empty()) {
            geometryCode = readShaderFile(geometryPath);
        }

        if (!tessControlPath.empty() && tessControlCode.empty()) {
            tessControlCode = readShaderFile(tessControlPath);
        }
        if (!tessEvalPath.empty() && tessEvalCode.empty()) {
            tessEvalCode = readShaderFile(tessEvalPath);
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    const GLuint vertex = compileShader(vertexCode, GL_VERTEX_SHADER);
    const GLuint fragment = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    GLuint geometry = 0;
    if (!geometryPath.empty()) {
        geometry = compileShader(geometryCode, GL_GEOMETRY_SHADER);
    }

    GLuint tessControl = 0;
    if (!tessControlPath.empty()) {
        tessControl = compileShader(tessControlCode, GL_TESS_CONTROL_SHADER);
    }

    GLuint tessEval = 0;
    if (!tessEvalPath.empty()) {
        tessEval = compileShader(tessEvalCode, GL_TESS_EVALUATION_SHADER);
    }

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    if (!geometryPath.empty()) {
        glAttachShader(ID, geometry);
    }
    if (!tessControlPath.empty()) {
        glAttachShader(ID, tessControl);
    }
    if (!tessEvalPath.empty()) {
        glAttachShader(ID, tessEval);
    }

    glLinkProgram(ID);
    checkCompileErrors(ID, true);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if (!geometryPath.empty()) {
        glDeleteShader(geometry);
    }
    if (!tessControlPath.empty()) {
        glDeleteShader(tessControl);
    }
    if (!tessEvalPath.empty()) {
        glDeleteShader(tessEval);
    }
}