#pragma once
#include "IShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderPredefineUtility.h"

namespace Graphics
{
   namespace OpenGL
   {

      class VertexFactoryShader :
         public IShader
      {
         using Base = IShader;

         std::string mShaderSource;

         std::vector<ShaderGenericDefineConstant> mConstantDefines;
         std::vector<ShaderGenericDefine> mDefines;

      public:

         VertexFactoryShader(const std::string& vertexFactoryName);

         virtual ~VertexFactoryShader();

         virtual void ProcessAllPredefines() override;
         virtual void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         std::string GetShaderSource() const;

         template <typename ValueType>
         void DefineConstant(const std::string& name, ValueType&& value)
         {
            std::string formatedValue = MacroConverter<ValueType>::GetValue(std::forward<ValueType>(value));
            mConstantDefines.emplace_back(ShaderGenericDefineConstant(name, formatedValue));
         }

         void Define(const std::string& name);

         void Undefine(const std::string& name);

      protected:

         void InitShader(const std::string& pathToShaderSource);

      };

   }
}

