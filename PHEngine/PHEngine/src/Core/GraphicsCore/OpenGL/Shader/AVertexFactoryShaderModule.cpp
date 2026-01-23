#include "AVertexFactoryShaderModule.h"

#include "Core/GameCore/LoggerExtension.h"

using namespace EngineCore;

namespace Graphics {
namespace OpenGL {
AVertexFactoryShaderModule::AVertexFactoryShaderModule(
    const CompositeShaderParams& shaderParams, std::shared_ptr<VertexFactoryShader> vertexFactoryShader)
    : IShader(shaderParams.mShaderName)
    , mVertexFactoryShader(vertexFactoryShader)
{
}

void AVertexFactoryShaderModule::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    IShader::AccessAllUniformLocations(shaderProgramID);

    ext_assert(GetBaseShader(), "AVertexFactoryShaderModule::AccessAllUniformLocations: Base shader is null");

    mVertexFactoryShader->AccessAllUniformLocations(shaderProgramID);
    GetBaseShader()->AccessAllUniformLocations(shaderProgramID);
}

void AVertexFactoryShaderModule::ProcessAllPredefines()
{
    mVertexFactoryShader->ProcessAllPredefines();
    GetBaseShader()->ProcessAllPredefines();
}

bool AVertexFactoryShaderModule::AssembleShaderSource()
{
    const std::string vertexFactoryShaderSource = mVertexFactoryShader->GetShaderSource();

    ShaderParams shaderParams = GetBaseShader()->GetShaderParams();

    std::unordered_map<eShaderType, std::string> shaderSources;

    for (const auto& [shaderType, shaderFile] : shaderParams.ShaderFiles) {
        LogInfo("AVertexFactoryShaderModule::AssembleShaderSource: Loading shader file: ", shaderFile);
        auto shaderSource = LoadShaderSource(shaderFile);

        if (shaderType == eShaderType::VertexShader) {
            // Vertex Factory shader is combined with vertex shader
            shaderSources[shaderType] = vertexFactoryShaderSource + "\n" + shaderSource;
        } else {
            shaderSources[shaderType] = shaderSource;
        }

        ProcessShaderIncludes(shaderSources[shaderType]);
    }
    return SendToGpuShadersSources(shaderSources);
}

void AVertexFactoryShaderModule::Init()
{
    ProcessAllPredefines();

    const bool bShaderLoadedSuccessfully = AssembleShaderSource();
    if (bShaderLoadedSuccessfully) {
        CompileShaders();
        m_shaderProgramID = glCreateProgram();
        BindAttributeLocations(m_shaderProgramID);
        LinkShaders();
        AccessAllUniformLocations(m_shaderProgramID);
    }
}

#if DEBUG

void AVertexFactoryShaderModule::RecompileShader()
{
    CleanUp(false);
    Init();
}

#endif
} // namespace OpenGL
} // namespace Graphics