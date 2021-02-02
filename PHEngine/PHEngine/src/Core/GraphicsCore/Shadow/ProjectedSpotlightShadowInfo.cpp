#include "ProjectedSpotlightShadowInfo.h"

namespace Graphics
{
   ProjectedSpotlightShadowInfo::ProjectedSpotlightShadowInfo(const TextureAtlasSpaceRequest& shadowAtlasCellResource)
      : ProjectedShadowInfo(shadowAtlasCellResource)
   {
      m_lightType = LightType::SPOT_LIGHT;
      Event::TextureAtlasGeneratedEvent::GetInstance()->AddListener(this);
   }

   ProjectedSpotlightShadowInfo::~ProjectedSpotlightShadowInfo()
   {
      Event::TextureAtlasGeneratedEvent::GetInstance()->RemoveListener(this);
   }

   std::shared_ptr<Texture2dAtlasHandler> ProjectedSpotlightShadowInfo::GetTexture2dHandler() const
   {
      return std::static_pointer_cast<Texture2dAtlasHandler>(mShadowmapHandler);
   }

   void ProjectedSpotlightShadowInfo::ProcessEvent(typename const Event::TextureAtlasGeneratedEvent::EventData_t& data)
   {
      if (TextureType::TEXTURE_2D == std::get<0>(data))
      {
         mShadowmapHandler = TextureAtlasFactory::GetInstance()->GetTextureAtlasCellByRequestId(mShadowmapAtlasRequest.MyRequestId);
         Event::TextureAtlasGeneratedEvent::GetInstance()->RemoveListener(this);
      }
   }

   void ProjectedSpotlightShadowInfo::BindShadowFramebuffer(bool clearDepthBuffer) const
   {
      ProjectedShadowInfo::BindShadowFramebuffer(clearDepthBuffer);

      auto rezolution = mShadowmapHandler->GetAtlasResource()->GetTextureRezolution();
      const GLbitfield clearDepthFlag = GL_DEPTH_BUFFER_BIT;
      m_shadowFramebuffer->RenderToTexture(0, 0, rezolution.x, rezolution.y, clearDepthFlag);
   }

   glm::mat4x4 ProjectedSpotlightShadowInfo::GetShadowViewMatrix() const
   {
      return mShadowViewMatrix;
   }

   glm::mat4x4 ProjectedSpotlightShadowInfo::GetShadowProjectionMatrix() const
   {
      return mShadowProjectionMatrix;
   }

   void ProjectedSpotlightShadowInfo::SetShadowViewMatrix(const glm::mat4x4& shadowViewMatrix)
   {
      mShadowViewMatrix = shadowViewMatrix;
   }

   void ProjectedSpotlightShadowInfo::SetShadowProjectionMatrix(const glm::mat4x4& shadowProjectionMatrix)
   {
      mShadowProjectionMatrix = shadowProjectionMatrix;
   }

   glm::mat4x4 ProjectedSpotlightShadowInfo::GetShadowMatrix() const
   {
      glm::mat4 shadowMatrix(1);
      shadowMatrix *= m_shadowBiasMatrix;
      shadowMatrix *= mShadowProjectionMatrix;
      shadowMatrix *= mShadowViewMatrix;
      return shadowMatrix;
   }

   glm::vec4 ProjectedSpotlightShadowInfo::GetTextureAtlasOffset() const
   {
      glm::vec4 result;

      if (mShadowmapHandler)
      {
         result = GetTexture2dHandler()->GetTextureAtlasOffset();
      }

      return result;
   }
}
