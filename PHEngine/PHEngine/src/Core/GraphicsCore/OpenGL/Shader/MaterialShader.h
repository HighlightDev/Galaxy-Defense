#pragma once

#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderPredefineUtility.h"

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

         std::shared_ptr<IMaterial> mMaterialInstance;

      public:

         IMaterialShader(std::shared_ptr<IMaterial> materialInstance);

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

         virtual void SetUniformValues(std::shared_ptr<IMaterial> materialInstance) = 0;

         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

      private:

         void InitMaterialShader(const std::string& pathToMaterialShader);
         void LoadMaterialShaderSource(const std::string& relativePathToMaterialShader);
      };

      class MaterialShaderImp
         : public IMaterialShader
      {

         std::unordered_map<std::string, Uniform> UniformsMap;

      public :
         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         virtual void SetUniformValues(std::shared_ptr<IMaterial> materialInstance) override;

         MaterialShaderImp(std::shared_ptr<IMaterial> materialInstance);
      };
   }
}

