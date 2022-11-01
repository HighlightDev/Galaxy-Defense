#include "ShaderPredefine.h"


namespace Graphics
{
	namespace OpenGL
	{
      /* ************** ShaderDefineConstant ************** */
      ShaderDefineConstant::ShaderDefineConstant(const std::string& name, const std::string& value, eShaderType shaderType)
         : ShaderGenericDefineConstant(name, value)
         , m_ShaderType(shaderType)
      {
      }

      /* ************** ShaderDefine ************** */

      ShaderDefine::ShaderDefine(const std::string& name, const bool isDefined, eShaderType shaderType)
         : ShaderGenericDefine(name, isDefined)
         , m_ShaderType(shaderType)
      {
      }
	}
}
