#pragma once

#include "ShaderType.h"

namespace Graphics {
namespace OpenGL {
struct ShaderParams {
    std::string ShaderName;
    std::unordered_map<eShaderType, std::string> ShaderFiles;

    std::unordered_map<uint64_t, std::pair<eShaderType, std::string>> ShaderCodeSnippets;

    explicit ShaderParams(const std::string& shaderName);

    explicit ShaderParams();

    void SetMainShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

    void SetGeometryShader(const std::string& geometryShaderFile);

    void
    SetTesselationShaders(const std::string& tesselationControlShaderFile, const std::string& tesselationEvaluationShaderFile);

    void SetComputeShader(const std::string& computeShaderFile);

    bool IsComputeShader() const;

    void AddShaderCodeSnippet(const uint64_t snippetId, const eShaderType shaderType, const std::string& shaderCodeSnippet);

    friend struct std::hash<ShaderParams>;

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
