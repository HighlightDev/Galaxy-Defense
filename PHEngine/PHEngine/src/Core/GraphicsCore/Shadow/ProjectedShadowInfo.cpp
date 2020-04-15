#include "ProjectedShadowInfo.h"

namespace Graphics
{

   ProjectedShadowInfo::ProjectedShadowInfo(const LazyTextureAtlasObtainer& shadowAtlasCellResource)
      : m_shadowAtlasCellResource(shadowAtlasCellResource)
      , m_shadowFramebuffer(nullptr)
      , m_shadowBiasMatrix(
         0.5f, 0, 0, 0,
         0, 0.5f, 0, 0,
         0, 0, 0.5f, 0,
         0.5f, 0.5f, 0.5f, 1)
      , mPlayerPositionOffset(0)
      , bShadowmapDirty(true)
   {
   }

   ProjectedShadowInfo::~ProjectedShadowInfo()
   {
      DeallocateFramebuffer();
      m_shadowAtlasCellResource.DeallocateTextureAtlasByRequestId();
   }

   void ProjectedShadowInfo::BindShadowFramebuffer(bool clearDepthBuffer) const
   {
      AllocateFramebuffer();
   }

   void ProjectedShadowInfo::AllocateFramebuffer() const
   {
      if (!m_shadowFramebuffer)
      {
         m_shadowFramebuffer = std::make_shared<ShadowFramebuffer>(m_shadowAtlasCellResource.GetTextureAtlasCellResource()->GetAtlasResource());
         m_shadowFramebuffer->UnbindFramebuffer();
      }
   }

   void ProjectedShadowInfo::DeallocateFramebuffer() const
   {
      m_shadowFramebuffer->CleanUp();
   }

   std::shared_ptr<ITexture> ProjectedShadowInfo::GetAtlasResource() const {

      std::shared_ptr<ITexture> result;
      auto resourceObtainer = m_shadowAtlasCellResource.GetTextureAtlasCellResource();
      if (resourceObtainer)
      {
         result = resourceObtainer->GetAtlasResource();
      }
      return result;
   }

   LightType ProjectedShadowInfo::GetLightType() const
   {
      return m_lightType;
   }

   void ProjectedShadowInfo::SetIsShadowMapDirty(const bool bDirty) {
      bShadowmapDirty = bDirty;
   }

   void ProjectedShadowInfo::SetPlayerPositionOffset(const glm::vec3& offset)
   {
      mPlayerPositionOffset = offset;
   }

   glm::vec3 ProjectedShadowInfo::GetPlayerPositionOffset() const {
      return mPlayerPositionOffset;
   }

   bool ProjectedShadowInfo::IsShadowMapDirty() const
   {
      return bShadowmapDirty;
   }
}
