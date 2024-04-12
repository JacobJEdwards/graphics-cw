//
// Created by Jacob Edwards on 08/04/2024.
//

#ifndef CW_UNIFORMBUFFER_H
#define CW_UNIFORMBUFFER_H

#include <GL/glew.h>
#include <string>
#include <memory>
#include "utils/Shader.h"
#include "utils/ShaderManager.h"


class UniformBuffer {
public:
    UniformBuffer();

    ~UniformBuffer();

    void bind() const;

    static void unbind() ;

    static void Create(const std::shared_ptr<Shader> &shader, const std::string &blockName, const GLuint bindingPoint) {
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
    void setSubData(const T &data, const GLuint offset) {
        bind();
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), &data);
        unbind();
    }

    static void setUniformBlockBinding(const std::shared_ptr<Shader> &shader, const std::string &blockName, const GLuint
    bindingPoint) {
        const GLuint blockIndex = glGetUniformBlockIndex(shader->getProgramID(), blockName.c_str());
        glUniformBlockBinding(shader->getProgramID(), blockIndex, bindingPoint);
    }

    static void setUniformBlockBinding(const std::string &shaderName, const std::string &blockName, const GLuint
    bindingPoint) {
        const auto shader = ShaderManager::Get(shaderName);
        const GLuint blockIndex = glGetUniformBlockIndex(shader->getProgramID(), blockName.c_str());
        glUniformBlockBinding(shader->getProgramID(), blockIndex, bindingPoint);
    }

    void bindBufferBase(const GLuint bindingPoint) const {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
    }

    void bindBufferRange(const GLuint bindingPoint, const GLuint offset, const GLuint size) const {
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, UBO, offset, size);
    }

    static void unbindBufferBase(const GLuint bindingPoint) {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, 0);
    }

    static void unbindBufferRange(const GLuint bindingPoint, const GLuint offset, const GLuint size) {
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, 0, offset, size);
    }

private:
    GLuint UBO = 0;
};


#endif //CW_UNIFORMBUFFER_H
