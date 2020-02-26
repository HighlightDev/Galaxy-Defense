#include "ShaderBase.h"

#include <iostream>

namespace Graphics
{
	namespace OpenGL
	{
		ShaderBase::ShaderBase(const ShaderParams& params)
			: Shader(params)
		{
		}

		ShaderBase::~ShaderBase()
		{
		}
	}
}