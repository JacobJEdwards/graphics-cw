//
// Created by Jacob Edwards on 20/03/2024.
//

#include "ShaderManager.h"

#include "utils/Shader.h"
#include <memory>
#include "imgui/imgui.h"

std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderManager::Shaders;

void ShaderManager::Add(const std::string &name, const std::filesystem::path &vertexPath,
                        const std::filesystem::path &fragmentPath, const std::filesystem::path &geometryPath,
                        const std::filesystem::path &tessControlPath, const std::filesystem::path &tessEvalPath) {

    Shaders[name] = std::make_shared<Shader>(vertexPath, fragmentPath, geometryPath, tessControlPath, tessEvalPath);
}

auto ShaderManager::Get(const std::string &name) -> std::shared_ptr<Shader> {
    auto it = Shaders.find(name);

    return it != Shaders.end() ? it->second : nullptr;
}

void ShaderManager::Remove(const std::string &name) {
    Shaders.erase(name);
}

void ShaderManager::Clear() {
    Shaders.clear();
}

auto ShaderManager::GetActiveShader() -> GLuint {
    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    return static_cast<GLuint>(currentProgram);
}

void ShaderManager::Interface() {
    ImGui::Begin("Shader Manager");
    ImGui::Text("Current Shader: %d", GetActiveShader());
    if (ImGui::Button("Reload all shaders")) {
        for (const auto &[name, shader]: Shaders) {
            shader->reload();
        }
    }
    ImGui::End();
}