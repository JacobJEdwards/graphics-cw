//
// Created by Jacob Edwards on 08/04/2024.
//

#ifndef CW_UNIFORMBUFFER_H
#define CW_UNIFORMBUFFER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <memory>
#include "utils/Shader.h"
#include "utils/ShaderManager.h"


class UniformBuffer {
public:
    UniformBuffer();

    ~UniformBuffer();

    void bind() const;

    void unbind() const;

    static void Create(std::shared_ptr<Shader> shader, const std::string &blockName, GLuint bindingPoint) {
        const GLuint blockIndex = glGetUniformBlockIndex(shader->getProgramID(), blockName.c_str());
        glUniformBlockBinding(shader->getProgramID(), blockIndex, bindingPoint);
    }

    template<typename T>
    void setData(const T &data) {
        bind();
        glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &data, GL_STATIC_DRAW);
        unbind();
    }

    template<typename T>
    void setSubData(const T &data, GLuint offset) {
        bind();
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), &data);
        unbind();
    }

    void setUniformBlockBinding(std::shared_ptr<Shader> shader, const std::string &blockName, GLuint bindingPoint) {
        const GLuint blockIndex = glGetUniformBlockIndex(shader->getProgramID(), blockName.c_str());
        glUniformBlockBinding(shader->getProgramID(), blockIndex, bindingPoint);
    }

    void setUniformBlockBinding(const std::string &shaderName, const std::string &blockName, GLuint bindingPoint) {
        const auto shader = ShaderManager::Get(shaderName);
        const GLuint blockIndex = glGetUniformBlockIndex(shader->getProgramID(), blockName.c_str());
        glUniformBlockBinding(shader->getProgramID(), blockIndex, bindingPoint);
    }

    void bindBufferBase(GLuint bindingPoint) {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
    }

    void bindBufferRange(GLuint bindingPoint, GLuint offset, GLuint size) {
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, UBO, offset, size);
    }

    void unbindBufferBase(GLuint bindingPoint) {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, 0);
    }

    void unbindBufferRange(GLuint bindingPoint, GLuint offset, GLuint size) {
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, 0, offset, size);
    }

private:
    GLuint UBO;
};


#endif //CW_UNIFORMBUFFER_H
