#pragma once

#include <memory>
#include <string>

#include "Core/GraphicsCore/OpenGL/Shader/ShaderBase.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

using namespace Graphics::OpenGL;
using namespace EngineUtility;

namespace Resources
{

   template <typename Model>
	class ShaderAllocationPolicy
	{
	public:
      using model_t = Model;

		template <typename ShaderT>
      static std::shared_ptr<ShaderBase> AllocateMemory(Model& arg)
      {
         return std::shared_ptr<ShaderBase>(std::make_shared<ShaderT>(arg));
      }

		static void DeallocateMemory(std::shared_ptr<ShaderBase> arg)
		{
			arg->CleanUp(true);
		}
	};

}