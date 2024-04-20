//
// Created by Jacob Edwards on 20/03/2024.
//

#include "ShaderManager.h"

#include "graphics/Shader.h"
#include <algorithm>
#include <filesystem>
#include <memory>
#include "imgui/imgui.h"
#include <ranges>
#include <string>
#include <unordered_map>

void ShaderManager::add(const std::string &name, const std::filesystem::path &vertexPath,
                        const std::filesystem::path &fragmentPath, const std::filesystem::path &geometryPath,
                        const std::filesystem::path &tessControlPath, const std::filesystem::path &tessEvalPath) {
    shaders[name] = std::make_shared<Shader>(vertexPath, fragmentPath, geometryPath, tessControlPath, tessEvalPath);
}

auto ShaderManager::get(const std::string &name) -> std::shared_ptr<Shader> {
    const auto it = shaders.find(name);

    return it != shaders.end() ? it->second : nullptr;
}

auto ShaderManager::get(const GLuint id) -> std::shared_ptr<Shader> {
    for (const auto &[name, shader]: shaders) {
        if (shader->getProgramID() == id) {
            return shader;
        }
    }

    return nullptr;
}

auto ShaderManager::getAll() -> std::unordered_map<std::string, std::shared_ptr<Shader> > & {
    return shaders;
}

void ShaderManager::remove(const std::string &name) {
    shaders.erase(name);
}

void ShaderManager::clear() {
    shaders.clear();
}

auto ShaderManager::GetActiveShader() -> GLuint {
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    return static_cast<GLuint>(currentProgram);
}

void ShaderManager::interface() {
    ImGui::Begin("Shader Manager");
    ImGui::Text("Current Shader: %d", GetActiveShader());

    if (ImGui::Button("Reload all shaders")) {
        std::ranges::for_each(shaders, [](const auto &shader) {
            shader.second->reload();
        });
    }
    ImGui::End();
}
