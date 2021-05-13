#pragma once

#include "PoolBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"
#include "Core/ResourceManagerCore/Policy/CompositeShaderAllocationPolicy.h"

using namespace Graphics::OpenGL;

namespace Resources
{

   class CompositeShaderPool 
      : public PoolBase<IShader, CompositeShaderParams, CompositeShaderAllocationPolicy>
   {
   public:

      using poolType_t = PoolBase<IShader, CompositeShaderParams, CompositeShaderAllocationPolicy>;

      static CompositeShaderPool* GetInstance()
      {
         static CompositeShaderPool instance;
         return &instance;
      }

#if DEBUG
      void RecompileShaders()
      {
         for (const auto& shader_pair : resourceMap)
         {
            shader_pair.second->RecompileShader();
         }
      }
#endif

   };

}