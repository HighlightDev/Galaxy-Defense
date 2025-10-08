#include "MaterialShader.h"

#include "Core/CommonCore/EngineConstants.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringStreamWrapper.h"

#include <algorithm>
#include <type_traits>
#include <unordered_set>

namespace Graphics {
namespace OpenGL {
/************************************************************************/
/*                                 MaterialShader                      */
/************************************************************************/
MaterialShader::MaterialShader(std::shared_ptr<MaterialProxy> materialProxy)
    : IShader(materialProxy->MaterialName)
    , mUniformNames(materialProxy->GetUniformNames())
    , mUniformArrayNames(materialProxy->GetUniformArrayNames())
{
    InitMaterialShader(materialProxy->MaterialShaderRelativePath);
}

MaterialShader::~MaterialShader()
{
}

void MaterialShader::ProcessAllPredefines()
{
    if (mConstantDefines.size() > 0 || mDefines.size() > 0) {
        if ("" != mShaderSource) {
            std::vector<ShaderGenericConstantArray> defineConstantArrays;
            ProcessPredefineToSource(mShaderSource, mConstantDefines, mDefines, defineConstantArrays);
        }
    }
}

void MaterialShader::InitMaterialShader(const std::string& relativePathToMaterialShader)
{
    LoadMaterialShaderSource(relativePathToMaterialShader);
}

void MaterialShader::LoadMaterialShaderSource(const std::string& relativePathToMaterialShader)
{
    mShaderSource = LoadShaderSource(relativePathToMaterialShader);
}

std::string MaterialShader::GetShaderSource() const
{
    return mShaderSource;
}

void MaterialShader::Define(const std::string& name)
{
    mDefines.emplace_back(name, true);
}

void MaterialShader::Undefine(const std::string& name)
{
    mDefines.emplace_back(name, false);
}

void MaterialShader::AccessAllUniformLocations(uint32_t shaderProgramID)
{
    for (const auto& name : mUniformNames) {
        Uniforms.emplace_back(GetUniform(name, shaderProgramID));
    }
    for (const auto& name : mUniformArrayNames) {
        UniformArrays.emplace_back(
            GetUniformArray(name, EngineConstants::c_maxInstancesPerInstanceBatch, shaderProgramID, eShaderType::FragmentShader));
    }
}

void MaterialShader::LoadUniformValues(const std::shared_ptr<MaterialProxy>& materialProxy, ActiveBindedState& activeBindedState)
{
    int32_t uIndex = 0;

    const std::unordered_set<MaterialProperty::eMaterialPropertyType> c_arrayUniformTypes
        = {MaterialProperty::eMaterialPropertyType::FLOAT_INSTANCED_PROPERTY};

    for (const auto& property : materialProxy->GetProperties()) {
        if (c_arrayUniformTypes.count(property->GetPropertyType())) {
            auto uniformArrayIt = std::find_if(UniformArrays.begin(), UniformArrays.end(), [&](const auto& uniformArray) {
                return uniformArray.GetUniformName() == property->GetPropertyName();
            });
            assert(uniformArrayIt != UniformArrays.end());
            property->SetValueToUniformArray(*uniformArrayIt);
        } else {
            auto uniformIt = std::find_if(Uniforms.begin(), Uniforms.end(), [&](const auto& uniform) {
                return uniform.GetUniformName() == property->GetPropertyName();
            });
            assert(uniformIt != Uniforms.end());
            property->SetValueToUniform(activeBindedState, *uniformIt, uIndex++);
        }
    }
}
} // namespace OpenGL
} // namespace Graphics
