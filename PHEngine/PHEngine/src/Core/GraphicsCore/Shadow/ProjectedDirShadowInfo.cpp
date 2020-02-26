#include "ProjectedDirShadowInfo.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasHandler.h"

namespace Graphics
{

   ProjectedDirShadowInfo::ProjectedDirShadowInfo(const LazyTextureAtlasObtainer& shadowAtlasCellResource)
      : ProjectedShadowInfo(shadowAtlasCellResource)
   {
      m_lightType = LightType::DIRECTIONAL_LIGHT;
   }

   ProjectedDirShadowInfo::~ProjectedDirShadowInfo()
   {
   }

   std::shared_ptr<Texture2dAtlasHandler> ProjectedDirShadowInfo::GetTexture2dHandler() const
   {
      std::shared_ptr<Texture2dAtlasHandler> result;

      auto resourceObtainer = m_shadowAtlasCellResource.GetTextureAtlasCellResource();
      if (resourceObtainer && resourceObtainer->GetTextureType() == TextureType::TEXTURE_2D)
      {
         result = std::static_pointer_cast<Texture2dAtlasHandler>(resourceObtainer);
      }

      return result;
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

   glm::vec4 ProjectedDirShadowInfo::GetPosOffsetShadowMapAtlas() const
   {
      glm::vec4 resultOffset;
     
      auto texAtlas = GetTexture2dHandler();

      if (texAtlas)
      {
         float x = static_cast<float>(texAtlas->GetAtlasCell().X);
         float y = static_cast<float>(texAtlas->GetAtlasCell().Y);
         float width = static_cast<float>(texAtlas->GetAtlasCell().Width);
         float height = static_cast<float>(texAtlas->GetAtlasCell().Height);

         float invShadowMapWidth = 1.0f / static_cast<float>(texAtlas->GetAtlasCell().TotalShadowMapWidth);
         float invShadowMapHeight = 1.0f / static_cast<float>(texAtlas->GetAtlasCell().TotalShadowMapHeight);

         float x_start = x * invShadowMapWidth;
         float y_start = y * invShadowMapHeight;
         float x_offset = width * invShadowMapWidth;
         float y_offset = height * invShadowMapHeight;

         resultOffset = glm::vec4(x_start, y_start, x_offset, y_offset);
      }

      return resultOffset;
   }
}
