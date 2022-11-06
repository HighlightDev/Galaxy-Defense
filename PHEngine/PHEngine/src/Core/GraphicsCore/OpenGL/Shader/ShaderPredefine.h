#pragma once
#include <string>

#include "ShaderType.h"

namespace Graphics
{
   namespace OpenGL
   {
      struct ShaderGenericDefineConstant
      {
         std::string m_Name;
         std::string m_Value;

         ShaderGenericDefineConstant(const std::string &name, const std::string &value);
      };

      struct ShaderGenericDefine
      {
         std::string m_Name;
         bool bDefined;

         ShaderGenericDefine(const std::string &name, const bool isDefined);
      };

      struct ShaderGenericConstantArray
      {
         std::string m_Name;
         std::string m_InnerTypeName;
         std::string m_Value;

         ShaderGenericConstantArray(const std::string &name, const std::string &innerTypeName, const std::string &value);
      };

      struct ShaderDefineConstant
          : public ShaderGenericDefineConstant
      {
         eShaderType m_ShaderType;
         ShaderDefineConstant(const std::string &name, const std::string &value, const eShaderType shaderType);
      };

      struct ShaderConstantArray
          : public ShaderGenericConstantArray
      {
         eShaderType m_ShaderType;
         ShaderConstantArray(const std::string &name, const std::string &innerTypeName, const std::string &value, const eShaderType shaderType);
      };

      struct ShaderDefine
          : public ShaderGenericDefine
      {

         eShaderType m_ShaderType;
         ShaderDefine(const std::string &name, const bool isDefined, const eShaderType shaderType);
      };
   }
}
