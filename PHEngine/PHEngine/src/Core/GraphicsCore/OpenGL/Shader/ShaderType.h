#pragma once
#include <stdint.h>
#include <gl\glew.h>

namespace Graphics
{
	namespace OpenGL
	{
		enum ShaderType : int32_t
		{
			VertexShader = 0x01,
			FragmentShader = 0x02,
			GeometryShader = 0x04,
		};
	}
}