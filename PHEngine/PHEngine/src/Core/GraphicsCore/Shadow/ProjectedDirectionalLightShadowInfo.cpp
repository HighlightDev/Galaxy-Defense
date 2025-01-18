#include "ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasHandler.h"

namespace Graphics
{

   ProjectedDirectionalLightShadowInfo::ProjectedDirectionalLightShadowInfo(const TextureAtlasSpaceRequest &shadowmapAtlasRequest,
                                                                            const float shadowOrthoHalfExtent)
       : ProjectedShadowInfo(shadowmapAtlasRequest),
         mShadowOrthoHalfExtent(shadowOrthoHalfExtent)
   {
      m_lightType = LightType::DIRECTIONAL_LIGHT;
   }

   ProjectedDirectionalLightShadowInfo::~ProjectedDirectionalLightShadowInfo()
   {
      Event::TextureAtlasGeneratedGameThreadEvent::GetInstance()->RemoveListener(Event::TextureAtlasGeneratedGameThreadEvent::GetInstanceId());
   }

   void ProjectedDirectionalLightShadowInfo::Initialize()
   {
      Event::TextureAtlasGeneratedGameThreadEvent::GetInstance()->AddListener(std::dynamic_pointer_cast<ProjectedDirectionalLightShadowInfo>(shared_from_this()));
   }

   std::shared_ptr<Texture2dAtlasHandler> ProjectedDirectionalLightShadowInfo::GetTexture2dHandler() const
   {
      return std::static_pointer_cast<Texture2dAtlasHandler>(mShadowmapHandler);
   }

   void ProjectedDirectionalLightShadowInfo::ProcessEvent(const TextureAtlasGeneratedGameThreadEvent* sender, const typename Event::TextureAtlasGeneratedGameThreadEvent::EventData_t &data)
   {
      if (eTextureType::TEXTURE_2D == std::get<0>(data))
      {
         mShadowmapHandler = TextureAtlasFactory::GetInstance()->GetTextureAtlasCellByRequestId(mShadowmapAtlasRequest.MyRequestId);
      }
   }

   glm::mat4 ProjectedDirectionalLightShadowInfo::GetShadowViewMatrix() const
   {
      return m_shadowViewMatrix;
   }

   glm::mat4 ProjectedDirectionalLightShadowInfo::GetShadowProjectionMatrix() const
   {
      return m_shadowProjectionMatrix;
   }

   void ProjectedDirectionalLightShadowInfo::SetShadowViewMatrix(const glm::mat4 &shadowViewMatrix)
   {
      m_shadowViewMatrix = shadowViewMatrix;
   }

   void ProjectedDirectionalLightShadowInfo::SetShadowProjectionMatrix(const glm::mat4 &shadowProjectionMatrix)
   {
      m_shadowProjectionMatrix = shadowProjectionMatrix;
   }

   glm::mat4 ProjectedDirectionalLightShadowInfo::GetShadowMatrix() const
   {
      glm::mat4 shadowMatrix(1);
      shadowMatrix *= m_shadowBiasMatrix;
      shadowMatrix *= m_shadowProjectionMatrix;
      shadowMatrix *= m_shadowViewMatrix;
      return shadowMatrix;
   }

   void ProjectedDirectionalLightShadowInfo::BindShadowFramebuffer(bool bBindFramebuffer, bool clearDepthBuffer) const
   {
      ProjectedShadowInfo::BindShadowFramebuffer(bBindFramebuffer, clearDepthBuffer);

      auto texAtlas = GetTexture2dHandler();
      auto atlas_cell = texAtlas->GetAtlasCell();
      const GLbitfield clearDepthFlag = clearDepthBuffer ? GL_DEPTH_BUFFER_BIT : 0;
      m_shadowFramebuffer->RenderToTexture(bBindFramebuffer, atlas_cell.X, atlas_cell.Y, atlas_cell.Width, atlas_cell.Height, clearDepthFlag);
   }

   glm::vec4 ProjectedDirectionalLightShadowInfo::GetTextureAtlasOffset() const
   {
      glm::vec4 result;

      if (mShadowmapHandler)
      {
         result = GetTexture2dHandler()->GetTextureAtlasOffset();
      }

      return result;
   }
}
