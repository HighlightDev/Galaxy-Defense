#pragma once
#include <cstddef>
#include <stdint.h>
#include <gl/glew.h>

#include "Core/CommonCore/Assertion.h"

namespace Graphics
{
	namespace OpenGL
	{
		enum eShaderType : int32_t
		{
			VertexShader = 0x01,
			FragmentShader = VertexShader << 1,
			GeometryShader = FragmentShader << 1,
			TesselationControlShader = GeometryShader << 1,
			TesselationEvaluationShader = TesselationControlShader << 1,
		};

		int32_t MapShaderTypeToOpenGLConstant(const eShaderType internalShaderType);
	}
}