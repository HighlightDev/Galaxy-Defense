#include "MaterialShader.h"

#include "Core/CommonCore/EngineConstants.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"
#include "Core/GraphicsCore/Material/MaterialProperties/TextureMaterialProperty.h"
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
            ModifyShaderSourceWithExtraData(
                mShaderSource, mConstantDefines, mDefines, defineConstantArrays, std::vector<ShaderCodeSnippet>());
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
            ext_assert(
                uniformArrayIt != UniformArrays.end(),
                "IShader::LoadUniformValues: Uniform array not found: " + property->GetPropertyName());
            property->SetValueToUniformArray(*uniformArrayIt);
        } else {
            if (property->GetPropertyType() == MaterialProperty::eMaterialPropertyType::TEXTURE_PROPERTY) {
                // additionally set isGrayscale uniform for texture properties
                auto isGrayscaleUniformIt = std::find_if(Uniforms.begin(), Uniforms.end(), [&](const auto& uniform) {
                    return uniform.GetUniformName() == property->GetPropertyName() + "_isGrayscale";
                });
                ext_assert(
                    isGrayscaleUniformIt != Uniforms.end(),
                    "IShader::LoadUniformValues: isGrayscale uniform not found: " + property->GetPropertyName() + "_isGrayscale");
                const auto textureProperty = std::dynamic_pointer_cast<TextureMaterialProperty>(property);
                ext_assert(
                    textureProperty,
                    "IShader::LoadUniformValues: Failed to cast to TextureMaterialProperty: " + property->GetPropertyName());
                isGrayscaleUniformIt->LoadUniform(textureProperty->IsGrayscale());
            }

            auto uniformIt = std::find_if(Uniforms.begin(), Uniforms.end(), [&](const auto& uniform) {
                return uniform.GetUniformName() == property->GetPropertyName();
            });
            ext_assert(
                uniformIt != Uniforms.end(), "IShader::LoadUniformValues: Uniform not found: " + property->GetPropertyName());
            property->SetValueToUniform(activeBindedState, *uniformIt, uIndex++);
        }
    }
}
} // namespace OpenGL
} // namespace Graphics
