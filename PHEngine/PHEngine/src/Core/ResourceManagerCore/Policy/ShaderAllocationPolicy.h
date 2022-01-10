#pragma once

#include <memory>
#include <string>

#include "Core/GraphicsCore/OpenGL/Shader/Shader.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

using namespace Graphics::OpenGL;
using namespace EngineUtility;

namespace Resources
{

	struct ShaderAllocationPolicy
	{
	  template <typename ShaderT, typename ShaderParamsModelType>
      static std::shared_ptr<Shader> AllocateMemory(const ShaderParamsModelType& arg)
      {
         return std::shared_ptr<Shader>(std::make_shared<ShaderT>(arg));
      }

		static void DeallocateMemory(std::shared_ptr<Shader> arg)
		{
			arg->CleanUp(true);
		}
	};

}