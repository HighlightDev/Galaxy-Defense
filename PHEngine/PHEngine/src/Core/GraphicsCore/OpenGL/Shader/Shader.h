#pragma once
#include "Core/GameCore/LoggerExtension.h"
#include "IShader.h"
#include "ShaderParams.h"
#include "ShaderPredefine.h"
#include "ShaderType.h"
#include "ShaderUtilityFunctions.h"

#include <vector>

using namespace EngineCore;

namespace Graphics {
namespace OpenGL {
class Shader : public IShader {
public:
    Shader(const ShaderParams& params);

    virtual ~Shader();

    bool operator==(const Shader& right) const;

private:
    ShaderParams m_shaderParams;

    std::vector<ShaderDefineConstant> m_defineConstantParameters;
    std::vector<ShaderDefine> m_defines;
    std::vector<ShaderConstantArray> m_defineConstantArrays;

protected:
    void LoadSubroutineIndex(const int32_t shaderType, int32_t countIndices, uint32_t* subroutineIndices) const;

    uint32_t GetSubroutineIndex(const int32_t shaderType, const std::string& subroutineName) const;

    template<typename ValueType>
    void DefineConstant(eShaderType shaderType, const std::string& name, const ValueType& value)
    {
        const auto formatedValue = MacroConverter<ValueType>::GetValue(value);
        m_defineConstantParameters.emplace_back(name, formatedValue, shaderType);
    }

    template<typename ArrayInternalType>
    void DefineConstantArray(
        const eShaderType shaderType, const std::string& varName, const std::vector<ArrayInternalType>& vectorValue)
    {
        const auto formatedValue = MacroConverter<ArrayInternalType>::GetArrayValue(varName, vectorValue);
        const auto innerTypeName = TypeToString<ArrayInternalType>::value;
        m_defineConstantArrays.emplace_back(varName, innerTypeName, formatedValue, shaderType);
    }

    void Define(eShaderType shaderType, const std::string& name);

    void Undefine(eShaderType shaderType, const std::string& name);

private:
    bool LoadShadersSourceToGpu();

public:
    // Init should be called in child constructor!
    virtual void ShaderInit();

    ShaderParams GetShaderParams() const;

    void ProcessAllPredefines() override;
#if DEBUG
    void RecompileShader() override;
#endif
};
} // namespace OpenGL
} // namespace Graphics