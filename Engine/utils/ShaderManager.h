#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils/Shader.h"

#include <unordered_map>
#include <filesystem>
#include <string>

class ShaderManager {
public:
    static void Add(const std::string &name, const std::filesystem::path &vertexPath,
                    const std::filesystem::path &fragmentPath, const std::filesystem::path &geometryPath = "",
                    const std::filesystem::path &tessControlPath = "", const std::filesystem::path &tessEvalPath = "");

    static auto Get(const std::string &name) -> std::shared_ptr<Shader>;

    static void Remove(const std::string &name);

    static void Clear();


private:
    ShaderManager() = default;

    static std::unordered_map<std::string, std::shared_ptr<Shader>> Shaders;
};

#endif