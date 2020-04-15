#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "Core/GraphicsCore/TextureAtlas/TextureAtlasFactory.h"
#include "Core/GraphicsCore/TextureAtlas/LazyTextureAtlasObtainer.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GameCore/FramebufferImplementation/ShadowFramebuffer.h"

using namespace Graphics::Texture;
using namespace Game::FramebufferImpl;

namespace Graphics
{

   enum class LightType
   {
      UNDEFINED,
      DIRECTIONAL_LIGHT,
      POINT_LIGHT,
      SPOT_LIGHT
   };

   struct ProjectedShadowInfo
   {
   protected:

      LightType m_lightType;

      LazyTextureAtlasObtainer m_shadowAtlasCellResource;

      mutable std::shared_ptr<ShadowFramebuffer> m_shadowFramebuffer;

      glm::mat4 m_shadowBiasMatrix;

      bool bShadowmapDirty;

      glm::vec3 mPlayerPositionOffset;

   public:

      ProjectedShadowInfo(const LazyTextureAtlasObtainer & shadowAtlasCellResource);

      virtual ~ProjectedShadowInfo();

      virtual void BindShadowFramebuffer(bool clearDepthBuffer) const;

      void SetIsShadowMapDirty(const bool bDirty);

      void SetPlayerPositionOffset(const glm::vec3& offset);

      std::shared_ptr<ITexture> GetAtlasResource() const;

      LightType GetLightType() const;

      glm::vec3 GetPlayerPositionOffset() const;

      bool IsShadowMapDirty() const;

   protected:

      void AllocateFramebuffer() const;

      void DeallocateFramebuffer() const;
   };

}

