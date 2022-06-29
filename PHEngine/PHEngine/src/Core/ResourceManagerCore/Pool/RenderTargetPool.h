#pragma once
#include "PoolBase.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/ResourceManagerCore/Policy/RenderTargetAllocationPolicy.h"

#include <iterator>

using namespace Graphics::Texture;

namespace Resources
{
   class RenderTargetPool :
      public PoolBase<ITexture, TexParams, RenderTargetAllocationPolicy>
   {

      static std::unique_ptr<RenderTargetPool> m_instance;

   public:

      using poolType_t = PoolBase<ITexture, TexParams, RenderTargetAllocationPolicy>;

      virtual std::string ToString() const override;

      std::shared_ptr<ITexture> GetRenderTargetAt(size_t index) const;

      static std::unique_ptr<RenderTargetPool>& GetInstance();

      static void ReloadInstance();

   };
}

