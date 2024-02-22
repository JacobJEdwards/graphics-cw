//
// Created by Jacob Edwards on 20/02/2024.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <string>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>


#include "Shader.h"

Shader::Shader(std::string vertexPath, std::string fragmentPath,
               std::string geometryPath, std::string tessControlPath,
               std::string tessEvalPath) : vertexPath(std::move(vertexPath)), fragmentPath(std::move(fragmentPath)),
                                           geometryPath(std::move(geometryPath)),
                                           tessControlPath(std::move(tessControlPath)),
                                           tessEvalPath(std::move(tessEvalPath)) {
    load();
}

Shader::~Shader() {
    deleteProgram();
}

auto Shader::operator=(Shader &&other) noexcept -> Shader & {
    if (this != &other) {
        ID = other.ID;
        other.ID = 0;
    }
    return *this;
}

Shader::Shader(Shader &&other) noexcept : ID(other.ID) {
    other.ID = 0;
}

[[nodiscard]] auto Shader::validateProgram() const -> bool {
    glValidateProgram(ID);
    GLint success;
    glGetProgramiv(ID, GL_VALIDATE_STATUS, &success);
    if (success == 0) {
        GLint infoLogLength;
        glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &infoLogLength);
        std::vector<GLchar> infoLog(infoLogLength);
        glGetProgramInfoLog(ID, infoLogLength, nullptr, infoLog.data());
        std::cerr << "ERROR::SHADER::PROGRAM_VALIDATION_FAILED\n" << infoLog.data() << std::endl;
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

[[nodiscard]] auto Shader::getProgramID() const -> GLuint {
    return ID;
}

void Shader::setBool(const std::string &name, const GLboolean value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<GLint>(value));
}

void Shader::setInt(const std::string &name, const GLint value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, const GLfloat value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

void Shader::setVec2(const std::string &name, const float x, const float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

void Shader::setVec3(const std::string &name, const float x, const float y, const float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}

void Shader::setVec4(const std::string &name, const float x, const float y, const float z, const float w) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(mat));
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(mat));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(mat));
}


void Shader::checkCompileErrors(const GLuint shader, const std::string &type) {
    GLint success;
    std::array<GLchar, BUFFER_SIZE> infoLog{};

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == 0) {
            glGetShaderInfoLog(shader, BUFFER_SIZE, nullptr, infoLog.data());
            throw std::runtime_error(
                "Shader compilation error of type: " + type + "\n" + std::string(infoLog.data()));
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (success == 0) {
            glGetProgramInfoLog(shader, BUFFER_SIZE, nullptr, infoLog.data());
            throw std::runtime_error("Program linking error of type: " + type + "\n" + std::string(infoLog.data()));
        }
    }
}

void Shader::deleteProgram() const {
    if (ID != 0) {
        glDeleteProgram(ID);
        const GLenum error = glGetError();

        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL error after deleting program: " << error << std::endl;
        }
    }
}

void Shader::load() {
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::string tessControlCode;
    std::string tessEvalCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    std::ifstream tcShaderFile;
    std::ifstream teShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    tcShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    teShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();

        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // if geometry shader path is present, also load a geometry shader
        if (!geometryPath.empty()) {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }

        if (!tessControlPath.empty()) {
            tcShaderFile.open(tessControlPath);
            std::stringstream tcShaderStream;
            tcShaderStream << tcShaderFile.rdbuf();
            tcShaderFile.close();
            tessControlCode = tcShaderStream.str();
        }
        if (!tessEvalPath.empty()) {
            teShaderFile.open(tessEvalPath);
            std::stringstream teShaderStream;
            teShaderStream << teShaderFile.rdbuf();
            teShaderFile.close();
            tessEvalCode = teShaderStream.str();
        }
    } catch (std::ifstream::failure &e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    GLuint vertex;
    GLuint fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // if geometry shader is given, compile geometry shader
    GLuint geometry;
    if (!geometryPath.empty()) {
        const char *gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, nullptr);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    GLuint tessControl;
    if (!tessControlPath.empty()) {
        const char *tcShaderCode = tessControlCode.c_str();
        tessControl = glCreateShader(GL_TESS_CONTROL_SHADER);
        glShaderSource(tessControl, 1, &tcShaderCode, nullptr);
        glCompileShader(tessControl);
        checkCompileErrors(tessControl, "TESS_CONTROL");
    }

    GLuint tessEval;
    if (!tessEvalPath.empty()) {
        const char *teShaderCode = tessEvalCode.c_str();
        tessEval = glCreateShader(GL_TESS_EVALUATION_SHADER);
        glShaderSource(tessEval, 1, &teShaderCode, nullptr);
        glCompileShader(tessEval);
        checkCompileErrors(tessEval, "TESS_EVALUATION");
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
    checkCompileErrors(ID, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
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
