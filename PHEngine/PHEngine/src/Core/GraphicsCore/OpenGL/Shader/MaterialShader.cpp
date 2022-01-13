#include "MaterialShader.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/UtilityCore/StringStreamWrapper.h"

#include <fstream>
#include <type_traits>
#include <algorithm>

namespace Graphics
{
   namespace OpenGL
   {
      /************************************************************************/
      /*                                 MaterialShader                      */
      /************************************************************************/
      MaterialShader::MaterialShader(std::shared_ptr<MaterialProxy> materialProxy)
         : IShader(materialProxy->MaterialName)
         , mUniformNames(materialProxy->GetUniformNames())
      {
         InitMaterialShader(materialProxy->MaterialShaderRelativePath);
      }

      MaterialShader::~MaterialShader()
      {
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
         for (const auto& name : mUniformNames)
         {
            Uniforms.emplace_back(GetUniform(name, shaderProgramID));
         }
      }

      void MaterialShader::LoadUniformValues(std::shared_ptr<MaterialProxy> materialProxy)
      {
         size_t uIndex = 0;
         
         for (const auto& property : materialProxy->GetProperties())
         {
            auto uniformIt = std::find_if(Uniforms.begin(), Uniforms.end(),
               [&](const auto& uniform) { return uniform.GetUniformName() == property->GetPropertyName(); });
            property->SetValueToUniform(*uniformIt, uIndex++);
         }
      }
   }
}
