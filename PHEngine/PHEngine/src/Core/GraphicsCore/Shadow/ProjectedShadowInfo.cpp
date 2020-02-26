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
      , Offset(0)
      , bMustUpdateShadowmap(true)
   {
   }

   ProjectedShadowInfo::~ProjectedShadowInfo()
   {
      DeallocateFramebuffer();
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

}
