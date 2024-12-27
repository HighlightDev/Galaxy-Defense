#pragma once

#include "IShader.h"
#include "Core/GraphicsCore/OpenGL/Shader/ShaderUtilityFunctions.h"

#include <memory>

namespace Graphics
{
   namespace OpenGL
   {
      class AttributeDataBase;

      class VertexFactoryShader : public IShader
      {
         using Base = IShader;

         std::string mShaderSource;

         std::vector<ShaderGenericDefineConstant> mConstantDefines;
         std::vector<ShaderGenericConstantArray> mDefineConstantArrays;
         std::vector<ShaderGenericDefine> mDefines;

      public:
         VertexFactoryShader(const std::string &vertexFactoryName);

         virtual ~VertexFactoryShader();

         virtual std::vector<std::shared_ptr<AttributeDataBase>> GetVertexAttributes(const int32_t shaderProgramId) = 0;

         void ProcessAllPredefines() override;
         
         void AccessAllUniformLocations(uint32_t shaderProgramID) override;

         void BindAttributeLocations(const int32_t shaderProgramId) override;

         std::string GetShaderSource() const;

         template <typename ValueType>
         void DefineConstant(const std::string &name, ValueType value)
         {
            std::string formatedValue = MacroConverter<ValueType>::GetValue(std::forward<ValueType>(value));
            mConstantDefines.emplace_back(name, formatedValue);
         }

         template <typename ArrayInternalType>
         void DefineConstantArray(const eShaderType shaderType, const std::string &varName, const std::vector<ArrayInternalType> &vectorValue)
         {
            const auto formatedValue = MacroConverter<ArrayInternalType>::GetArrayValue(varName, vectorValue);
            const auto innerTypeName = TypeToString<ArrayInternalType>::value;
            mDefineConstantArrays.emplace_back(varName, innerTypeName, formatedValue, shaderType);
         }

         void Define(const std::string &name);

         void Undefine(const std::string &name);

      protected:
         void InitShader(const std::string &pathToShaderSource);
      };

   }
}
