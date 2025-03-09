#pragma once
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/UtilityCore/StringStreamWrapper.h"
#include "ShaderPredefine.h"
#include "ShaderType.h"
#include "Uniform.h"

#include <cstddef>
#include <string>

namespace Graphics {
namespace OpenGL {
class IShader {
protected:
    int32_t m_vertexShaderID;
    int32_t m_fragmentShaderID;
    int32_t m_geometryShaderID;
    uint32_t m_shaderProgramID;

    std::string mShaderName;

public:
    IShader(const std::string& shaderName);

    virtual ~IShader();

    bool operator==(const IShader& right) const;

    Uniform GetUniform(const std::string& uniformName, uint32_t shaderProgramID) const;

    UniformArray GetUniformArray(
        const std::string& uniformName, size_t countOfUniforms, uint32_t shaderProgramID, const eShaderType shaderType) const;

    int32_t GetAttributeLocationByName(const std::string& attributeName) const;

    std::string GetCompileLogInfo() const;

    std::string GetLinkLogInfo() const;

    uint32_t GetShaderProgramID() const;

    bool IsShaderCompiled() const;

    void ExecuteShader() const;

    void StopShader() const;

    void CleanUp(bool bDeleteShaderProgram = true);

    virtual void AccessAllUniformLocations(uint32_t shaderProgramID);
    virtual void AccessAllSubroutineIndices(uint32_t shaderProgramID);
    virtual void SetShaderPredefine() { };
    virtual void ProcessAllPredefines() { };
    virtual void BindAttributeLocations(const int32_t shaderProgramId)
    {
    }

#if DEBUG
    virtual void RecompileShader() { };
#endif

    void ProcessPredefineToFile(
        const std::string& pathToShader,
        const std::vector<ShaderGenericDefineConstant>& constantDefines,
        const std::vector<ShaderGenericDefine>& defines,
        const std::vector<ShaderGenericConstantArray>& constantArrays) const;

    void ProcessPredefineToSource(
        std::string& shaderSource,
        const std::vector<ShaderGenericDefineConstant>& constantDefines,
        const std::vector<ShaderGenericDefine>& defines,
        const std::vector<ShaderGenericConstantArray>& constantArrays) const;

    void WriteShaderSrc(const std::string& pathToShader, const std::string& src) const;

    void CompileShaders();
    void LinkShaders() const;

    bool ProcessShaderIncludes(std::string& shaderSource);
    bool SendToGpuSingleShaderSource(int32_t shaderId, const std::string& shaderSource) const;
    bool SendToGpuShadersSources(std::string& vsSource, std::string& gsSource, std::string& fsSource);

    std::vector<std::string> LoadShaderSrcVector(const std::string& pathToShader) const;
    std::string LoadShaderSource(const std::string& pathToShader) const;

private:
    std::string GetPredefinedSource(
        std::vector<std::string>& shaderSourceVector,
        const std::vector<ShaderGenericDefineConstant>& constantDefines,
        const std::vector<ShaderGenericDefine>& defines,
        const std::vector<ShaderGenericConstantArray>& constantArrays) const;
};
} // namespace OpenGL
} // namespace Graphics