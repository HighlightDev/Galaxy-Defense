#include "ProjectedDirShadowInfo.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasHandler.h"

namespace Graphics
{

   ProjectedDirShadowInfo::ProjectedDirShadowInfo(const TextureAtlasSpaceRequest& shadowmapAtlasRequest,
      const float shadowOrthoHalfExtent)
      : ProjectedShadowInfo(shadowmapAtlasRequest)
      , mShadowOrthoHalfExtent(shadowOrthoHalfExtent)
   {    
      m_lightType = LightType::DIRECTIONAL_LIGHT;
      Event::TextureAtlasGeneratedEvent::GetInstance()->AddListener(this);
   }

   ProjectedDirShadowInfo::~ProjectedDirShadowInfo()
   {
      Event::TextureAtlasGeneratedEvent::GetInstance()->RemoveListener(this);
   }

   std::shared_ptr<Texture2dAtlasHandler> ProjectedDirShadowInfo::GetTexture2dHandler() const
   {
      return std::static_pointer_cast<Texture2dAtlasHandler>(mShadowmapHandler);
   }

   void ProjectedDirShadowInfo::ProcessEvent(typename const Event::TextureAtlasGeneratedEvent::EventData_t& data)
   {
      if (TextureType::TEXTURE_2D == std::get<0>(data))
      {
         mShadowmapHandler = TextureAtlasFactory::GetInstance()->GetTextureAtlasCellByRequestId(mShadowmapAtlasRequest.MyRequestId);

         Event::TextureAtlasGeneratedEvent::GetInstance()->RemoveListener(this);
      }

      Event::TextureAtlasGeneratedEvent::GetInstance()->RemoveListener(this);
   }

   glm::mat4 ProjectedDirShadowInfo::GetShadowViewMatrix() const
   {
      return m_shadowViewMatrix;
   }

   glm::mat4 ProjectedDirShadowInfo::GetShadowProjectionMatrix() const
   {
      return m_shadowProjectionMatrix;
   }

   void ProjectedDirShadowInfo::SetShadowViewMatrix(const glm::mat4& shadowViewMatrix)
   {
      m_shadowViewMatrix = shadowViewMatrix;
   }

   void ProjectedDirShadowInfo::SetShadowProjectionMatrix(const glm::mat4& shadowProjectionMatrix)
   {
      m_shadowProjectionMatrix = shadowProjectionMatrix;
   }

   glm::mat4 ProjectedDirShadowInfo::GetShadowMatrix() const
   {
      glm::mat4 shadowMatrix(1);
      shadowMatrix *= m_shadowBiasMatrix;
      shadowMatrix *= m_shadowProjectionMatrix;
      shadowMatrix *= m_shadowViewMatrix;
      return shadowMatrix;
   }

   void ProjectedDirShadowInfo::BindShadowFramebuffer(bool clearDepthBuffer) const
   {
      ProjectedShadowInfo::BindShadowFramebuffer(clearDepthBuffer);

      auto texAtlas = GetTexture2dHandler();
      auto atlas_cell = texAtlas->GetAtlasCell();
      const GLbitfield clearDepthFlag = clearDepthBuffer ? GL_DEPTH_BUFFER_BIT : 0;
      m_shadowFramebuffer->RenderToFBO(1, atlas_cell.X, atlas_cell.Y, atlas_cell.Width, atlas_cell.Height, clearDepthFlag);
      glDrawBuffer(GL_NONE);
   }

   glm::vec4 ProjectedDirShadowInfo::GetTextureAtlasOffset() const
   {
      glm::vec4 result;

      if (mShadowmapHandler)
      {
         result = GetTexture2dHandler()->GetTextureAtlasOffset();
      }

      return result;
   }
}
