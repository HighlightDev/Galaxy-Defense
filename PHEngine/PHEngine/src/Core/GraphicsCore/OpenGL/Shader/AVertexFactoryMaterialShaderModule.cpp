#include "AVertexFactoryMaterialShaderModule.h"

namespace Graphics {
namespace OpenGL {
AVertexFactoryMaterialShaderModule::AVertexFactoryMaterialShaderModule(
    const CompositeShaderParams& shaderParams,
    std::shared_ptr<VertexFactoryShader> vertexFactoryShader,
    std::shared_ptr<MaterialShader> materialShader)
    : AVertexFactoryShaderModule(shaderParams, vertexFactoryShader)
    , mMaterialShader(std::move(materialShader))
{
}

AVertexFactoryMaterialShaderModule::~AVertexFactoryMaterialShaderModule()
{
}

std::shared_ptr<MaterialShader> AVertexFactoryMaterialShaderModule::GetMaterialShader() const
{
    return mMaterialShader;
}

void AVertexFactoryMaterialShaderModule::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    AVertexFactoryShaderModule::AccessAllUniformLocations(shaderProgramID);
    mMaterialShader->AccessAllUniformLocations(shaderProgramID);
}

void AVertexFactoryMaterialShaderModule::ProcessAllPredefines()
{
    AVertexFactoryShaderModule::ProcessAllPredefines();
    mMaterialShader->ProcessAllPredefines();
}

bool AVertexFactoryMaterialShaderModule::AssembleShaderSource()
{
    ext_assert(GetBaseShader(), "AVertexFactoryMaterialShaderModule::AssembleShaderSource: Base shader is null");

    const std::string vertexFactoryShaderSource = mVertexFactoryShader->GetShaderSource();
    const std::string materialShaderSource = mMaterialShader->GetShaderSource();

    const ShaderParams& shaderParams = GetBaseShader()->GetShaderParams();

    std::unordered_map<eShaderType, std::string> shaderSources;

    for (const auto& [shaderType, shaderFile] : shaderParams.ShaderFiles) {
        LogInfo("AVertexFactoryMaterialShaderModule::AssembleShaderSource: Loading shader file: ", shaderFile);
        auto shaderSource = LoadShaderSource(shaderFile);

        if (shaderType == eShaderType::VertexShader) {
            // Vertex Factory shader is combined with vertex shader
            shaderSources[shaderType] = vertexFactoryShaderSource + "\n" + shaderSource;
        } else if (shaderType == eShaderType::FragmentShader) {
            // Material shader is combined with fragment shader
            shaderSources[shaderType] = materialShaderSource + shaderSource;
        } else {
            shaderSources[shaderType] = shaderSource;
        }

        ProcessShaderIncludes(shaderSources[shaderType]);
    }

    return SendToGpuShadersSources(shaderSources);
}
} // namespace OpenGL
} // namespace Graphics