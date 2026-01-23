#pragma once

#include "ShaderType.h"

namespace Graphics {
namespace OpenGL {
struct ShaderParams {
    std::string ShaderName;
    std::unordered_map<eShaderType, std::string> ShaderFiles;

    explicit ShaderParams(const std::string& shaderName);

    explicit ShaderParams();

    void SetMainShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);

    void SetGeometryShader(const std::string& geometryShaderFile);

    void
    SetTesselationShaders(const std::string& tesselationControlShaderFile, const std::string& tesselationEvaluationShaderFile);

    void SetComputeShader(const std::string& computeShaderFile);

    bool IsComputeShader() const;

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
