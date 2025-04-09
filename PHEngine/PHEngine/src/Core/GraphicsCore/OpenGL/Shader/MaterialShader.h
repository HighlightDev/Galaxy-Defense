#pragma once

#include "Core/GraphicsCore/Material/MaterialProxy.h"
#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderUtilityFunctions.h"
#include "Core/GraphicsCore/Renderer/ActiveBindedState.h"

#include <vector>

using namespace Graphics;

namespace Graphics {
namespace OpenGL {
class MaterialShader : public IShader {

    std::string mShaderSource;

    std::vector<ShaderGenericDefineConstant> mConstantDefines;
    std::vector<ShaderGenericDefine> mDefines;

protected:
    std::vector<std::string> mUniformNames;

    std::vector<std::string> mUniformArrayNames;

    std::vector<Uniform> Uniforms;

    std::vector<UniformArray> UniformArrays;

public:
    virtual ~MaterialShader();

    MaterialShader(std::shared_ptr<MaterialProxy> materialProxy);

    std::string GetShaderSource() const;

    virtual void LoadUniformValues(const std::shared_ptr<MaterialProxy>& materialProxy, ActiveBindedState& activeBindedState);

    void AccessAllUniformLocations(uint32_t shaderProgramID) override;

    template<typename ValueType>
    void DefineConstant(const std::string& name, ValueType&& value)
    {
        std::string formatedValue = MacroConverter<ValueType>::GetValue(std::forward<ValueType>(value));
        mConstantDefines.emplace_back(ShaderGenericDefineConstant(name, formatedValue));
    }

    void Define(const std::string& name);

    void Undefine(const std::string& name);

private:
    void InitMaterialShader(const std::string& pathToMaterialShader);
    void LoadMaterialShaderSource(const std::string& relativePathToMaterialShader);
};
} // namespace OpenGL
} // namespace Graphics
