#pragma once
#include "ProjectedShadowInfo.h"

namespace Graphics
{

   class ProjectedDirShadowInfo :
      public ProjectedShadowInfo
   {
      glm::mat4x4 m_shadowViewMatrix;

      glm::mat4x4 m_shadowProjectionMatrix;

      std::shared_ptr<Texture2dAtlasHandler>  GetTexture2dHandler() const;

   public:      

      ProjectedDirShadowInfo(const LazyTextureAtlasObtainer& shadowAtlasCellResource);

      virtual ~ProjectedDirShadowInfo();

      virtual void BindShadowFramebuffer(bool clearDepthBuffer) const override;

      glm::mat4 GetShadowViewMatrix() const;

      glm::mat4 GetShadowProjectionMatrix() const;

      void SetShadowViewMatrix(const glm::mat4& shadowViewMatrix);

      void SetShadowProjectionMatrix(const glm::mat4& shadowProjectionMatrix);

      glm::mat4 GetShadowMatrix() const;

      glm::vec4 GetPosOffsetShadowMapAtlas() const;
   };

}

