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

      std::string ToString() const override;

      static CompositeShaderPool* GetInstance();

#if DEBUG
      void RecompileShaders();
#endif

   };

}