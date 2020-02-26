#pragma once

#include "PoolBase.h"
#include "Core/GraphicsCore/OpenGL/Shader/IShader.h"
#include "Core/ResourceManagerCore/Policy/CompositeShaderAllocationPolicy.h"

using namespace Graphics::OpenGL;

namespace Resources
{

   class CompositeShaderPool : public PoolBase<IShader, CompositeShaderParams, CompositeShaderAllocationPolicy>
   {
      static std::unique_ptr<CompositeShaderPool> m_instance;

   public:

      using poolType_t = PoolBase<IShader, CompositeShaderParams, CompositeShaderAllocationPolicy>;

      static std::unique_ptr<CompositeShaderPool>& GetInstance()
      {
         if (!m_instance)
            m_instance = std::make_unique<CompositeShaderPool>();

         return m_instance;
      }

      static void ReloadInstance()
      {
         if (m_instance)
            m_instance.reset();
      }
   };

}