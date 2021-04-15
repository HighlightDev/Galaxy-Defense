#pragma once

#include "Core/GraphicsCore/Material/MaterialProxy.h"
#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderPredefineUtility.h"

#include <vector>

namespace Graphics
{
   namespace OpenGL
   {
      class IMaterialShader 
         : public IShader
      {
         std::string mShaderSource;

         std::vector<ShaderGenericDefineConstant> mConstantDefines;
         std::vector<ShaderGenericDefine> mDefines;

      protected:

         std::vector<std::string> mUniformNames;

      public:

         IMaterialShader(const std::string& materialName, const std::string& materialShaderRelativePath, const std::vector<std::string>& uniformNames);

         virtual ~IMaterialShader();

         std::string GetShaderSource() const;

         template <typename ValueType>
         void DefineConstant(const std::string& name, ValueType&& value)
         {
            std::string formatedValue = MacroConverter<ValueType>::GetValue(std::forward<ValueType>(value));
            mConstantDefines.emplace_back(ShaderGenericDefineConstant(name, formatedValue));
         }

         void Define(const std::string& name);

         void Undefine(const std::string& name);

         virtual void LoadUniformValues(std::shared_ptr<MaterialProxy> materialProxy) = 0;

         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

      private:

         void InitMaterialShader(const std::string& pathToMaterialShader);
         void LoadMaterialShaderSource(const std::string& relativePathToMaterialShader);
      };

      class MaterialShaderImp
         : public IMaterialShader
      {

         std::vector<Uniform> Uniforms;

      public :
         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         virtual void LoadUniformValues(std::shared_ptr<MaterialProxy> materialProxy) override;

         MaterialShaderImp(const std::string& materialName, const std::string& materialShaderRelativePath, const std::vector<std::string>& uniformNames);
      };
   }
}

