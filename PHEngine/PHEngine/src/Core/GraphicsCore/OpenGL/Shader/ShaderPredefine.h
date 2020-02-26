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

         ShaderGenericDefineConstant(const std::string& name, const std::string& value)
            : m_Name(name)
            , m_Value(value)
         {
         }
      };

      struct ShaderGenericDefine
      {
         std::string m_Name;
         bool bDefined;

         ShaderGenericDefine(const std::string& name, const bool isDefined)
            : m_Name(name)
            , bDefined(isDefined)
         {
         }
      };

		struct ShaderDefineConstant
         : public ShaderGenericDefineConstant
		{
			ShaderType m_ShaderType;
			ShaderDefineConstant(const std::string& name, const std::string& value, ShaderType shaderType);
		};

      struct ShaderDefine
         : public ShaderGenericDefine
      {
     
         ShaderType m_ShaderType;
         ShaderDefine(const std::string& name, const bool isDefined, ShaderType shaderType);
      };
	}
}

