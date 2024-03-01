#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    // Set a uniform variable of type T
    template<typename T>
    void setUniform(const std::string& name, const T& value) const;

private:
    unsigned int ID; // Shader program ID
};

template<typename T>
void Shader::setUniform(const std::string& name, const T& value) const {
    static_assert(false, "Unsupported uniform type"); // Provide an error for unsupported types
}

// Specializations for supported types
template<>
void Shader::setUniform<bool>(const std::string& name, const bool& value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int>(value));
}

template<>
void Shader::setUniform<int>(const std::string& name, const int& value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

template<>
void Shader::setUniform<float>(const std::string& name, const float& value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

template<>
void Shader::setUniform<glm::vec2>(const std::string& name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

template<>
void Shader::setUniform<glm::vec3>(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

template<>
void Shader::setUniform<glm::vec4>(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

template<>
void Shader::setUniform<glm::mat2>(const std::string& name, const glm::mat2& value) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void Shader::setUniform<glm::mat3>(const std::string& name, const glm::mat3& value) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void Shader::setUniform<glm::mat4>(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

