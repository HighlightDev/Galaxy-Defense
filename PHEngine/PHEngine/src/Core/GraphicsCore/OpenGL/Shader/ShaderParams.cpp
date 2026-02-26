#include "ShaderParams.h"

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

void ShaderParams::SetMainShaders(const std::string& vertexShaderFile, const std::string& fragmentShaderFile)
{
    ShaderFiles[eShaderType::VertexShader] = vertexShaderFile;
    ShaderFiles[eShaderType::FragmentShader] = fragmentShaderFile;
}

void ShaderParams::SetGeometryShader(const std::string& geometryShaderFile)
{
    ShaderFiles[eShaderType::GeometryShader] = geometryShaderFile;
}

void ShaderParams::SetTesselationShaders(
    const std::string& tesselationControlShaderFile, const std::string& tesselationEvaluationShaderFile)
{
    ShaderFiles[eShaderType::TesselationControlShader] = tesselationControlShaderFile;
    ShaderFiles[eShaderType::TesselationEvaluationShader] = tesselationEvaluationShaderFile;
}

void ShaderParams::SetComputeShader(const std::string& computeShaderFile)
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

bool ShaderParams::operator==(const ShaderParams& other) const
{
    const std::string& vsFile = this->ShaderFiles.find(eShaderType::VertexShader) != this->ShaderFiles.end()
        ? this->ShaderFiles.at(eShaderType::VertexShader)
        : "";
    const std::string& fsFile = this->ShaderFiles.find(eShaderType::FragmentShader) != this->ShaderFiles.end()
        ? this->ShaderFiles.at(eShaderType::FragmentShader)
        : "";
    const std::string& gsFile = this->ShaderFiles.find(eShaderType::GeometryShader) != this->ShaderFiles.end()
        ? this->ShaderFiles.at(eShaderType::GeometryShader)
        : "";
    const std::string& tcsFile = this->ShaderFiles.find(eShaderType::TesselationControlShader) != this->ShaderFiles.end()
        ? this->ShaderFiles.at(eShaderType::TesselationControlShader)
        : "";
    const std::string& tesFile = this->ShaderFiles.find(eShaderType::TesselationEvaluationShader) != this->ShaderFiles.end()
        ? this->ShaderFiles.at(eShaderType::TesselationEvaluationShader)
        : "";
    const std::string& csFile = this->ShaderFiles.find(eShaderType::ComputeShader) != this->ShaderFiles.end()
        ? this->ShaderFiles.at(eShaderType::ComputeShader)
        : "";

    const auto& otherVsFile = other.ShaderFiles.find(eShaderType::VertexShader) != other.ShaderFiles.end()
        ? other.ShaderFiles.at(eShaderType::VertexShader)
        : "";
    const auto& otherFsFile = other.ShaderFiles.find(eShaderType::FragmentShader) != other.ShaderFiles.end()
        ? other.ShaderFiles.at(eShaderType::FragmentShader)
        : "";
    const auto& otherGsFile = other.ShaderFiles.find(eShaderType::GeometryShader) != other.ShaderFiles.end()
        ? other.ShaderFiles.at(eShaderType::GeometryShader)
        : "";
    const auto& otherTcsFile = other.ShaderFiles.find(eShaderType::TesselationControlShader) != other.ShaderFiles.end()
        ? other.ShaderFiles.at(eShaderType::TesselationControlShader)
        : "";
    const auto& otherTesFile = other.ShaderFiles.find(eShaderType::TesselationEvaluationShader) != other.ShaderFiles.end()
        ? other.ShaderFiles.at(eShaderType::TesselationEvaluationShader)
        : "";
    const auto& otherCsFile = other.ShaderFiles.find(eShaderType::ComputeShader) != other.ShaderFiles.end()
        ? other.ShaderFiles.at(eShaderType::ComputeShader)
        : "";

    const bool isShaderCodeSnippetsEqual = this->ShaderCodeSnippets.size() == other.ShaderCodeSnippets.size()
        && std::all_of(this->ShaderCodeSnippets.cbegin(), this->ShaderCodeSnippets.cend(), [&other](const auto& snippetPair) {
                                               return other.ShaderCodeSnippets.count(snippetPair.first) != 0;
                                           });

    return this->ShaderName == other.ShaderName && vsFile == otherVsFile && fsFile == otherFsFile && gsFile == otherGsFile
        && tcsFile == otherTcsFile && tesFile == otherTesFile && csFile == otherCsFile && isShaderCodeSnippetsEqual;
}
} // namespace OpenGL
} // namespace Graphics

namespace std {
using namespace Graphics::OpenGL;
std::size_t hash<ShaderParams>::operator()(const ShaderParams& k) const
{
    const auto& vsFile
        = k.ShaderFiles.find(eShaderType::VertexShader) != k.ShaderFiles.end() ? k.ShaderFiles.at(eShaderType::VertexShader) : "";
    const auto& fsFile = k.ShaderFiles.find(eShaderType::FragmentShader) != k.ShaderFiles.end()
        ? k.ShaderFiles.at(eShaderType::FragmentShader)
        : "";
    const auto& gsFile = k.ShaderFiles.find(eShaderType::GeometryShader) != k.ShaderFiles.end()
        ? k.ShaderFiles.at(eShaderType::GeometryShader)
        : "";
    const auto& tcsFile = k.ShaderFiles.find(eShaderType::TesselationControlShader) != k.ShaderFiles.end()
        ? k.ShaderFiles.at(eShaderType::TesselationControlShader)
        : "";
    const auto& tesFile = k.ShaderFiles.find(eShaderType::TesselationEvaluationShader) != k.ShaderFiles.end()
        ? k.ShaderFiles.at(eShaderType::TesselationEvaluationShader)
        : "";
    const auto& csFile = k.ShaderFiles.find(eShaderType::ComputeShader) != k.ShaderFiles.end()
        ? k.ShaderFiles.at(eShaderType::ComputeShader)
        : "";

    const auto shaderCodeSnippetsHash = std::accumulate(
        k.ShaderCodeSnippets.cbegin(), k.ShaderCodeSnippets.cend(), 0u, [](const auto& accumulatedHash, const auto& snippetPair) {
            return accumulatedHash ^ (hash<uint32_t>()(snippetPair.first));
        });

    return hash<std::string>()(k.ShaderName) ^ hash<std::string>()(vsFile) ^ hash<std::string>()(fsFile)
        ^ hash<std::string>()(gsFile) ^ hash<std::string>()(tcsFile) ^ hash<std::string>()(tesFile) ^ hash<std::string>()(csFile)
        ^ shaderCodeSnippetsHash;
}
} // namespace std
