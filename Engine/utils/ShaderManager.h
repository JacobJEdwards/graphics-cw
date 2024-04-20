#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <GL/glew.h>

#include "graphics/Shader.h"

#include <memory>
#include <unordered_map>
#include <filesystem>
#include <string>
#include "Singleton.h"

class ShaderManager final : public Singleton<ShaderManager> {
public:
    friend class Singleton<ShaderManager>;

    void add(const std::string &name, const std::filesystem::path &vertexPath,
             const std::filesystem::path &fragmentPath, const std::filesystem::path &geometryPath = "",
             const std::filesystem::path &tessControlPath = "", const std::filesystem::path &tessEvalPath = "");

    auto get(const std::string &name) -> std::shared_ptr<Shader>;

    auto get(GLuint id) -> std::shared_ptr<Shader>;

    auto getAll() -> std::unordered_map<std::string, std::shared_ptr<Shader> > &;

    void remove(const std::string &name);

    void clear();

    static auto GetActiveShader() -> GLuint;

    void interface();

    explicit ShaderManager(Token) {
    }

private:
    ShaderManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Shader> > shaders;
};

#endif
