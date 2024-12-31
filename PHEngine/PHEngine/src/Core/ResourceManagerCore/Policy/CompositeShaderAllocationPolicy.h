#pragma once

#include <memory>
#include <vector>

#include "Core/GraphicsCore/OpenGL/Shader/CompositeShaderParams.h"
#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"
#include "Core/GameCore/LoggerExtension.h"

using namespace Graphics::OpenGL;
using namespace EngineCore;

namespace Resources
{
   struct CompositeShaderAllocationPolicy
   {
      template <typename CompositeShaderType>
      static std::shared_ptr<IShader> AllocateMemory(const CompositeShaderParams& arg)
      {
         LogInfo("CompositeShaderAllocationPolicy::AllocateMemory: ", arg.mShaderName);
         return std::make_shared<CompositeShaderType>(arg);
      }

      static void DeallocateMemory(std::shared_ptr<IShader> arg)
      {
         LogInfo("CompositeShaderAllocationPolicy::DeallocateMemory");
         arg->CleanUp(true);
      }
   };

}