#pragma once
#include "Shader.h"

namespace Graphics
{
	namespace OpenGL
	{

		class ShaderBase :
			public Shader
		{
		public:
			
			ShaderBase(const ShaderParams& params);

			~ShaderBase();
		};

	}
}
