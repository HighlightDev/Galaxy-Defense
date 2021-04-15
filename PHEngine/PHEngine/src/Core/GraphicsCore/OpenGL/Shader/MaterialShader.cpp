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
      /*                                 IMaterialShader                      */
      /************************************************************************/
      IMaterialShader::IMaterialShader(const std::string& materialName, const std::string& materialShaderRelativePath, const std::vector<std::string>& uniformNames)
         : IShader(materialName)
         , mUniformNames(uniformNames)
      {
         InitMaterialShader(materialShaderRelativePath);
      }

      IMaterialShader::~IMaterialShader()
      {
      }

      void IMaterialShader::InitMaterialShader(const std::string& relativePathToMaterialShader)
      {
         LoadMaterialShaderSource(relativePathToMaterialShader);
      }

      void IMaterialShader::LoadMaterialShaderSource(const std::string& relativePathToMaterialShader)
      {
         std::string absoluteShaderPath = EngineUtility::ConvertFromRelativeToAbsolutePath(relativePathToMaterialShader);
         mShaderSource = LoadShaderSource(absoluteShaderPath);
      }

      std::string IMaterialShader::GetShaderSource() const
      {
         return mShaderSource;
      }

      void IMaterialShader::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         // This is an implementation in base class
      }

      void IMaterialShader::Define(const std::string& name)
      {
         mDefines.emplace_back(name, true);
      }

      void IMaterialShader::Undefine(const std::string& name)
      {
         mDefines.emplace_back(name, false);
      }

      /************************************************************************/
      /*                               MaterialShaderImpl                     */
      /************************************************************************/

      void MaterialShaderImp::AccessAllUniformLocations(uint32_t shaderProgramID)
      {
         for (const auto& name : mUniformNames)
         {
            Uniforms.emplace_back(GetUniform(name, shaderProgramID));
         }
      }

      void MaterialShaderImp::LoadUniformValues(std::shared_ptr<MaterialProxy> materialProxy)
      {
         size_t uIndex = 0;
         
         for (const auto& property : materialProxy->GetProperties())
         {
            auto uniformIt = std::find_if(Uniforms.begin(), Uniforms.end(),
               [&](const auto& uniform) { return uniform.GetUniformName() == property->GetPropertyName(); });
            property->SetValueToUniform(*uniformIt, uIndex++);
         }
      }

      MaterialShaderImp::MaterialShaderImp(const std::string& materialName, const std::string& materialShaderRelativePath,
         const std::vector<std::string>& uniformNames)
         : IMaterialShader(materialName, materialShaderRelativePath, uniformNames)
      {
      }
   }
}
