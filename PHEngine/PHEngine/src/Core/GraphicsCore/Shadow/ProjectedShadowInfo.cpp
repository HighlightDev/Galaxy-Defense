#include "ProjectedShadowInfo.h"

namespace Graphics
{

   ProjectedShadowInfo::ProjectedShadowInfo(const TextureAtlasSpaceRequest &shadowmapAtlasRequest)
       : mShadowmapAtlasRequest(shadowmapAtlasRequest), m_shadowFramebuffer(nullptr), m_shadowBiasMatrix(
                                                                                          0.5f, 0, 0, 0,
                                                                                          0, 0.5f, 0, 0,
                                                                                          0, 0, 0.5f, 0,
                                                                                          0.5f, 0.5f, 0.5f, 1),
         mPlayerPositionOffset(0), bShadowmapDirty(true), mShadowmapHandler(nullptr)
   {
   }

   ProjectedShadowInfo::~ProjectedShadowInfo()
   {
   }

   void ProjectedShadowInfo::BindShadowFramebuffer(bool bBindFramebuffer, bool clearDepthBuffer) const
   {
      AllocateFramebuffer();
   }

   void ProjectedShadowInfo::AllocateFramebuffer() const
   {
      if (!m_shadowFramebuffer && mShadowmapHandler)
      {
         m_shadowFramebuffer = std::make_shared<ShadowFramebuffer>(mShadowmapHandler->GetAtlasResource());
         m_shadowFramebuffer->UnbindFramebuffer(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      }
   }

   void ProjectedShadowInfo::DeallocateFramebuffer() const
   {
      m_shadowFramebuffer->CleanUp();
   }

   void ProjectedShadowInfo::CleanUp()
   {
      DeallocateFramebuffer();
      TextureAtlasFactory::GetInstance()->DeallocateTextureAtlasByRequestId(mShadowmapAtlasRequest.MyRequestId);
   }

   std::shared_ptr<ITexture> ProjectedShadowInfo::GetAtlasResource() const
   {

      std::shared_ptr<ITexture> result;

      if (mShadowmapHandler)
      {
         result = mShadowmapHandler->GetAtlasResource();
      }
      return result;
   }

   LightType ProjectedShadowInfo::GetLightType() const
   {
      return m_lightType;
   }

   void ProjectedShadowInfo::SetIsShadowMapDirty(const bool bDirty)
   {
      bShadowmapDirty = bDirty;
   }

   void ProjectedShadowInfo::SetPlayerPositionOffset(const glm::vec3 &offset)
   {
      mPlayerPositionOffset = offset;
   }

   glm::vec3 ProjectedShadowInfo::GetPlayerPositionOffset() const
   {
      return mPlayerPositionOffset;
   }

   bool ProjectedShadowInfo::IsShadowMapDirty() const
   {
      return bShadowmapDirty;
   }

   TextureAtlasSpaceRequest ProjectedShadowInfo::GetTextureAtlasSpaceRequest() const
   {
      return mShadowmapAtlasRequest;
   }
}
