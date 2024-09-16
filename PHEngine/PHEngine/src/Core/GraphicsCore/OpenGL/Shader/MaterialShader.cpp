#include "MaterialShader.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringStreamWrapper.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialProperty.h"

#include <type_traits>
#include <algorithm>
#include <unordered_set>

namespace Graphics
{
   namespace OpenGL
   {
      /************************************************************************/
      /*                                 MaterialShader                      */
      /************************************************************************/
      MaterialShader::MaterialShader(std::shared_ptr<MaterialProxy> materialProxy)
          : IShader(materialProxy->MaterialName),
            mUniformNames(materialProxy->GetUniformNames()),
            mUniformArrayNames(materialProxy->GetUniformArrayNames())
      {
         InitMaterialShader(materialProxy->MaterialShaderRelativePath);
      }

      MaterialShader::~MaterialShader()
      {
      }

      void MaterialShader::InitMaterialShader(const std::string &relativePathToMaterialShader)
      {
         LoadMaterialShaderSource(relativePathToMaterialShader);
      }

      void MaterialShader::LoadMaterialShaderSource(const std::string &relativePathToMaterialShader)
      {
         mShaderSource = LoadShaderSource(relativePathToMaterialShader);
      }

      std::string MaterialShader::GetShaderSource() const
      {
         return mShaderSource;
      }

      void MaterialShader::Define(const std::string &name)
      {
         mDefines.emplace_back(name, true);
      }

      void MaterialShader::Undefine(const std::string &name)
      {
         mDefines.emplace_back(name, false);
      }

      void MaterialShader::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         for (const auto &name : mUniformNames)
         {
            Uniforms.emplace_back(GetUniform(name, shaderProgramID));
         }
         static constexpr auto s_maxUniformArraySize = 200; // todo: for now
         for (const auto &name : mUniformArrayNames)
         {
            UniformArrays.emplace_back(GetUniformArray(name, s_maxUniformArraySize, shaderProgramID, eShaderType::FragmentShader));
         }
      }

      void MaterialShader::LoadUniformValues(std::shared_ptr<MaterialProxy> materialProxy)
      {
         int32_t uIndex = 0;

         const std::unordered_set<MaterialProperty::eMaterialPropertyType> c_arrayUniformTypes = {
             MaterialProperty::eMaterialPropertyType::FLOAT_INSTANCED_PROPERTY};

         for (const auto &property : materialProxy->GetProperties())
         {
            if (c_arrayUniformTypes.count(property->GetPropertyType()))
            {
               auto uniformArrayIt = std::find_if(UniformArrays.begin(), UniformArrays.end(),
                                                  [&](const auto &uniformArray)
                                                  { return uniformArray.GetUniformName() == property->GetPropertyName(); });
               assert(uniformArrayIt != UniformArrays.end());
               property->SetValueToUniformArray(*uniformArrayIt);
            }
            else
            {
               auto uniformIt = std::find_if(Uniforms.begin(), Uniforms.end(),
                                             [&](const auto &uniform)
                                             { return uniform.GetUniformName() == property->GetPropertyName(); });
               assert(uniformIt != Uniforms.end());
               property->SetValueToUniform(*uniformIt, uIndex++);
            }
         }
      }
   }
}
