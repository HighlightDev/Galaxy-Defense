#include "ProjectedPointShadowInfo.h"

namespace Graphics
{
   ProjectedPointShadowInfo::ProjectedPointShadowInfo(const TextureAtlasSpaceRequest& shadowAtlasCellResource)
      : ProjectedShadowInfo(shadowAtlasCellResource)
   {
      m_lightType = LightType::POINT_LIGHT;
      Event::TextureAtlasGeneratedEvent::GetInstance()->AddListener(this);
   }

   ProjectedPointShadowInfo::~ProjectedPointShadowInfo()
   {
      Event::TextureAtlasGeneratedEvent::GetInstance()->RemoveListener(this);
   }

   std::shared_ptr<TextureCubeAtlasHandler> ProjectedPointShadowInfo::GetTextureCubeHandler() const
   {
      return  std::static_pointer_cast<TextureCubeAtlasHandler>(mShadowmapHandler);
   }

   void ProjectedPointShadowInfo::ProcessEvent(typename const Event::TextureAtlasGeneratedEvent::EventData_t& data)
   {
      if (TextureType::TEXTURE_CUBE == std::get<0>(data))
      {
         mShadowmapHandler = TextureAtlasFactory::GetInstance()->GetTextureAtlasCellByRequestId(mShadowmapAtlasRequest.MyRequestId);
         Event::TextureAtlasGeneratedEvent::GetInstance()->RemoveListener(this);
      }
   }

   void ProjectedPointShadowInfo::BindShadowFramebuffer(bool clearDepthBuffer) const 
   {
      ProjectedShadowInfo::BindShadowFramebuffer(clearDepthBuffer);

      auto rezolution = mShadowmapHandler->GetAtlasResource()->GetTextureRezolution();
      const GLbitfield clearDepthFlag = GL_DEPTH_BUFFER_BIT;
      m_shadowFramebuffer->RenderToFBO(1, 0, 0, rezolution.x, rezolution.y, clearDepthFlag);
   }

   ProjectedPointShadowInfo::six_mat4x4 ProjectedPointShadowInfo::GetShadowViewMatrices() const
   {
      return m_shadowViewMatrix;
   }

   ProjectedPointShadowInfo::six_mat4x4 ProjectedPointShadowInfo::GetShadowProjectionMatrices() const
   {
      return m_shadowProjectionMatrix;
   }

   void ProjectedPointShadowInfo::SetShadowViewMatrices(const six_mat4x4& shadowViewMatrices)
   {
      m_shadowViewMatrix = shadowViewMatrices;
   }

   void ProjectedPointShadowInfo::SetShadowProjectionMatrix(const six_mat4x4& shadowProjectionMatrices)
   {
      m_shadowProjectionMatrix = shadowProjectionMatrices;
   }

   ProjectedPointShadowInfo::six_mat4x4 ProjectedPointShadowInfo::GetShadowMatrix() const
   {
      ProjectedPointShadowInfo::six_mat4x4 result;

      // 0
      glm::mat4 shadowMatrix(1);
      shadowMatrix *= m_shadowBiasMatrix;
      shadowMatrix *= m_shadowProjectionMatrix[0];
      shadowMatrix *= m_shadowViewMatrix[0];
      result[0] = shadowMatrix;
      // 1
      shadowMatrix = glm::mat4(1);
      shadowMatrix *= m_shadowBiasMatrix;
      shadowMatrix *= m_shadowProjectionMatrix[1];
      shadowMatrix *= m_shadowViewMatrix[1];
      result[1] = shadowMatrix;
      // 2
      shadowMatrix = glm::mat4(1);
      shadowMatrix *= m_shadowBiasMatrix;
      shadowMatrix *= m_shadowProjectionMatrix[2];
      shadowMatrix *= m_shadowViewMatrix[2];
      result[2] = shadowMatrix;
      // 3
      shadowMatrix = glm::mat4(1);
      shadowMatrix *= m_shadowBiasMatrix;
      shadowMatrix *= m_shadowProjectionMatrix[3];
      shadowMatrix *= m_shadowViewMatrix[3];
      std::get<3>(result) = shadowMatrix;
      // 4
      shadowMatrix = glm::mat4(1);
      shadowMatrix *= m_shadowBiasMatrix;
      shadowMatrix *= m_shadowProjectionMatrix[4];
      shadowMatrix *= m_shadowViewMatrix[4];
      std::get<4>(result) = shadowMatrix;
      // 5
      shadowMatrix = glm::mat4(1);
      shadowMatrix *= m_shadowBiasMatrix;
      shadowMatrix *= m_shadowProjectionMatrix[5];
      shadowMatrix *= m_shadowViewMatrix[5];
      std::get<5>(result) = shadowMatrix;

      return result;
   }
}
