#include "ShaderPredefine.h"

namespace Graphics
{
   namespace OpenGL
   {
      ShaderGenericDefineConstant::ShaderGenericDefineConstant(const std::string &name, const std::string &value)
          : m_Name(name),
            m_Value(value)
      {
      }

      ShaderGenericDefine::ShaderGenericDefine(const std::string &name, const bool isDefined)
          : m_Name(name),
            bDefined(isDefined)
      {
      }

      ShaderGenericConstantArray::ShaderGenericConstantArray(const std::string &name, const std::string &innerTypeName, const std::string &value)
          : m_Name(name),
            m_InnerTypeName(innerTypeName),
            m_Value(value)
      {
      }

      /* ************** ShaderDefineConstant ************** */
      ShaderDefineConstant::ShaderDefineConstant(const std::string &name, const std::string &value, eShaderType shaderType)
          : ShaderGenericDefineConstant(name, value),
            m_ShaderType(shaderType)
      {
      }

      ShaderConstantArray::ShaderConstantArray(const std::string &name, const std::string &innerTypeName, const std::string &value, const eShaderType shaderType)
          : ShaderGenericConstantArray(name, innerTypeName, value),
            m_ShaderType(shaderType)
      {
      }

      /* ************** ShaderDefine ************** */

      ShaderDefine::ShaderDefine(const std::string &name, const bool isDefined, eShaderType shaderType)
          : ShaderGenericDefine(name, isDefined), m_ShaderType(shaderType)
      {
      }
   }
}
