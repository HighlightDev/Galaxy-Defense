#pragma once

#include "ShaderType.h"

#include <filesystem>
#include <string>
#include <unordered_map>

namespace Graphics {
namespace OpenGL {
struct ShaderParams {
    std::string ShaderName;
    std::unordered_map<eShaderType, std::filesystem::path> ShaderFiles;

    std::unordered_map<uint64_t, std::pair<eShaderType, std::string>> ShaderCodeSnippets;

    explicit ShaderParams(const std::string& shaderName);

    explicit ShaderParams();

    void SetMainShaders(const std::filesystem::path& vertexShaderFile, const std::filesystem::path& fragmentShaderFile);

    void SetGeometryShader(const std::filesystem::path& geometryShaderFile);

    void SetTesselationShaders(
        const std::filesystem::path& tesselationControlShaderFile, const std::filesystem::path& tesselationEvaluationShaderFile);

    void SetComputeShader(const std::filesystem::path& computeShaderFile);

    bool IsComputeShader() const;

    void AddShaderCodeSnippet(const uint64_t snippetId, const eShaderType shaderType, const std::string& shaderCodeSnippet);

    friend struct std::hash<ShaderParams>;

    const std::filesystem::path& GetShaderFile(const eShaderType shaderType) const;

    bool operator==(const ShaderParams& other) const;
};
} // namespace OpenGL
} // namespace Graphics

namespace std {
using namespace Graphics::OpenGL;
template<>
struct hash<ShaderParams> {
    std::size_t operator()(const ShaderParams& k) const;
};
} // namespace std
