#include "ShaderPredefine.h"

#include <xutility>

namespace Graphics
{
	namespace OpenGL
	{
      /* ************** ShaderDefineConstant ************** */
      ShaderDefineConstant::ShaderDefineConstant(const std::string& name, const std::string& value, ShaderType shaderType)
         : ShaderGenericDefineConstant(name, value)
         , m_ShaderType(shaderType)
      {
      }

      /* ************** ShaderDefine ************** */

      ShaderDefine::ShaderDefine(const std::string& name, const bool isDefined, ShaderType shaderType)
         : ShaderGenericDefine(name, isDefined)
         , m_ShaderType(shaderType)
      {
      }
	}
}
