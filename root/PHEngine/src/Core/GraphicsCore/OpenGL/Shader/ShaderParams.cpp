#include "ShaderParams.h"

#include "Core/CommonCore/Assertion.h"

#include <algorithm>
#include <numeric>

namespace Graphics {
namespace OpenGL {
ShaderParams::ShaderParams(const std::string& shaderName)
    : ShaderName(shaderName)
    , ShaderFiles()
{
}

ShaderParams::ShaderParams()
    : ShaderName("")
    , ShaderFiles()
{
}

void ShaderParams::SetMainShaders(const std::filesystem::path& vertexShaderFile, const std::filesystem::path& fragmentShaderFile)
{
    ShaderFiles[eShaderType::VertexShader] = vertexShaderFile;
    ShaderFiles[eShaderType::FragmentShader] = fragmentShaderFile;
}

void ShaderParams::SetGeometryShader(const std::filesystem::path& geometryShaderFile)
{
    ShaderFiles[eShaderType::GeometryShader] = geometryShaderFile;
}

void ShaderParams::SetTesselationShaders(
    const std::filesystem::path& tesselationControlShaderFile, const std::filesystem::path& tesselationEvaluationShaderFile)
{
    ShaderFiles[eShaderType::TesselationControlShader] = tesselationControlShaderFile;
    ShaderFiles[eShaderType::TesselationEvaluationShader] = tesselationEvaluationShaderFile;
}

void ShaderParams::SetComputeShader(const std::filesystem::path& computeShaderFile)
{
    ShaderFiles[eShaderType::ComputeShader] = computeShaderFile;
}

bool ShaderParams::IsComputeShader() const
{
    return ShaderFiles.find(eShaderType::ComputeShader) != ShaderFiles.end();
}

void ShaderParams::AddShaderCodeSnippet(
    const uint64_t snippetId, const eShaderType shaderType, const std::string& shaderCodeSnippet)
{
    ext_assert(
        ShaderCodeSnippets.find(snippetId) == ShaderCodeSnippets.end(),
        "Shader code snippet with id " + std::to_string(snippetId) + " already exists in shader params with name " + ShaderName);
    ShaderCodeSnippets[snippetId] = std::make_pair(shaderType, shaderCodeSnippet);
}

const std::filesystem::path& ShaderParams::GetShaderFile(const eShaderType shaderType) const
{
    static const std::filesystem::path sEmptyPath;
    const auto it = ShaderFiles.find(shaderType);
    return it != ShaderFiles.end() ? it->second : sEmptyPath;
}

bool ShaderParams::operator==(const ShaderParams& other) const
{
    const auto isSameShaderFile = [this, &other](const eShaderType shaderType) {
        return this->GetShaderFile(shaderType) == other.GetShaderFile(shaderType);
    };

    const bool isShaderCodeSnippetsEqual = this->ShaderCodeSnippets.size() == other.ShaderCodeSnippets.size()
        && std::all_of(this->ShaderCodeSnippets.cbegin(), this->ShaderCodeSnippets.cend(), [&other](const auto& snippetPair) {
                                               return other.ShaderCodeSnippets.count(snippetPair.first) != 0;
                                           });

    return this->ShaderName == other.ShaderName && isSameShaderFile(eShaderType::VertexShader)
        && isSameShaderFile(eShaderType::FragmentShader) && isSameShaderFile(eShaderType::GeometryShader)
        && isSameShaderFile(eShaderType::TesselationControlShader) && isSameShaderFile(eShaderType::TesselationEvaluationShader)
        && isSameShaderFile(eShaderType::ComputeShader) && isShaderCodeSnippetsEqual;
}
} // namespace OpenGL
} // namespace Graphics

namespace std {
using namespace Graphics::OpenGL;
std::size_t hash<ShaderParams>::operator()(const ShaderParams& k) const
{
    const auto fileHash = [&k](const eShaderType shaderType) { return std::filesystem::hash_value(k.GetShaderFile(shaderType)); };

    const auto shaderCodeSnippetsHash = std::accumulate(
        k.ShaderCodeSnippets.cbegin(),
        k.ShaderCodeSnippets.cend(),
        static_cast<uint64_t>(0),
        [](const auto& accumulatedHash, const auto& snippetPair) {
            return accumulatedHash ^ (hash<uint64_t>()(snippetPair.first));
        });

    return hash<std::string>()(k.ShaderName) ^ fileHash(eShaderType::VertexShader) ^ fileHash(eShaderType::FragmentShader)
        ^ fileHash(eShaderType::GeometryShader) ^ fileHash(eShaderType::TesselationControlShader)
        ^ fileHash(eShaderType::TesselationEvaluationShader) ^ fileHash(eShaderType::ComputeShader) ^ shaderCodeSnippetsHash;
}
} // namespace std
