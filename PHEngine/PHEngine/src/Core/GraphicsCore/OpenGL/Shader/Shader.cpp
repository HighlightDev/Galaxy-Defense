#include "Shader.h"

#include "Core/GameCore/LoggerExtension.h"

#include <gl/glew.h>
#include <glm/vec2.hpp>

#include <fstream>

using namespace EngineCore;

namespace Graphics {
namespace OpenGL {

Shader::Shader(const ShaderParams& params)
    : IShader(params.ShaderName)
    , m_shaderParams(params)
    , m_defineConstantParameters()
    , m_defines()
    , m_defineConstantArrays()
{
    m_codeSnippets.reserve(params.ShaderCodeSnippets.size());
    for (const auto& [snippetId, codeSnippet] : params.ShaderCodeSnippets) {
        m_codeSnippets.emplace_back(codeSnippet.first, EngineUtility::Split(codeSnippet.second, '\n'));
    }
}

Shader::~Shader()
{
}

bool Shader::operator==(const Shader& right) const
{
    return this->mShaderName == right.mShaderName && this->m_shaderIdsMap == right.m_shaderIdsMap
        && this->m_shaderProgramID == right.m_shaderProgramID;
}

void Shader::ShaderInit()
{
    ext_assert(
        m_shaderProgramID == std::numeric_limits<uint32_t>::max(),
        "Attempt to initialize shader that has already been initialized. Shader name: " + mShaderName);

    SetShaderPredefine(); // start precompile shader customization
    ProcessAllPredefines();

    const bool bShaderLoadedSuccessfully = LoadShadersSourceToGpu();
    if (bShaderLoadedSuccessfully) {
        CompileShaders();
        m_shaderProgramID = glCreateProgram();
        LinkShaders();
        AccessAllUniformLocations(m_shaderProgramID);
        AccessAllSubroutineIndices(m_shaderProgramID);
    } else {
        LogInfo("Shader::ShaderInit: Shader source loading failed");
    }
}

const ShaderParams& Shader::GetShaderParams() const
{
    return m_shaderParams;
}

bool Shader::LoadShadersSourceToGpu()
{
    std::unordered_map<eShaderType, std::string> shaderSources;
    for (const auto& [shaderType, shaderFile] : m_shaderParams.ShaderFiles) {
        LogInfo("Shader::LoadShadersSourceToGpu: Loading shader file: ", shaderFile);

        auto shaderSource = LoadShaderSource(shaderFile);
        ProcessShaderIncludes(shaderSource);

        std::unordered_map<eShaderType, std::vector<ShaderGenericDefineConstant>> constantPredefines;
        std::unordered_map<eShaderType, std::vector<ShaderGenericConstantArray>> arrayConstants;
        std::unordered_map<eShaderType, std::vector<ShaderGenericDefine>> predefines;
        std::unordered_map<eShaderType, std::vector<ShaderCodeSnippet>> codeSnippets;

        for (const auto& define : m_defineConstantParameters) {
            constantPredefines[define.m_ShaderType].emplace_back(define);
        }

        for (const auto& arrayConstant : m_defineConstantArrays) {
            arrayConstants[arrayConstant.m_ShaderType].emplace_back(arrayConstant);
        }

        for (const auto& define_it : m_defines) {
            predefines[define_it.m_ShaderType].emplace_back(define_it);
        }

        for (const auto& codeSnippet : m_codeSnippets) {
            codeSnippets[codeSnippet.m_ShaderType].emplace_back(codeSnippet);
        }

        ModifyShaderSourceWithExtraData(
            shaderSource,
            constantPredefines[shaderType],
            predefines[shaderType],
            arrayConstants[shaderType],
            codeSnippets[shaderType]);

        shaderSources[shaderType] = shaderSource;
    }

    return SendToGpuShadersSources(shaderSources);
}

void Shader::ProcessAllPredefines()
{
}

uint32_t Shader::GetSubroutineIndex(const int32_t shaderType, const std::string& subroutineName) const
{
    const auto result = glGetSubroutineIndex(m_shaderProgramID, (GLenum)shaderType, subroutineName.c_str());
    if (result == GL_INVALID_INDEX) {
        LogInfo("Shader::GetSubroutineIndex: subroutine " + subroutineName + " not found in shader " + m_shaderParams.ShaderName);
    }
    return result;
}

void Shader::LoadSubroutineIndex(const int32_t shaderType, int32_t countIndices, uint32_t* subroutineIndices) const
{
    glUniformSubroutinesuiv((GLenum)shaderType, countIndices, subroutineIndices);
}

#if DEBUG

void Shader::RecompileShader()
{
    CleanUp(false);
    const bool bLoaded = LoadShadersSourceToGpu();

    if (bLoaded) {
        CompileShaders();
        LinkShaders();
        AccessAllUniformLocations(m_shaderProgramID);
    }

    const bool bCompiledSuccesfully = IsShaderCompiled();
    LogInfo(
        "Shader::RecompileShader: shader " + m_shaderParams.ShaderName
        + (bCompiledSuccesfully ? " has recompiled successfully " : "has not recompiled"));
}

#endif

void Shader::Define(eShaderType shaderType, const std::string& name)
{
    m_defines.emplace_back(ShaderDefine(name, true, shaderType));
}

void Shader::Undefine(eShaderType shaderType, const std::string& name)
{
    m_defines.emplace_back(ShaderDefine(name, false, shaderType));
}

} // namespace OpenGL

} // namespace Graphics