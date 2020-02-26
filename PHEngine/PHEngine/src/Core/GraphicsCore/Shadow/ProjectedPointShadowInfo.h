#pragma once
#include "ProjectedShadowInfo.h"

#include <array>

namespace Graphics
{

   class ProjectedPointShadowInfo :
      public ProjectedShadowInfo
   {
   public:

      using six_mat4x4 = std::array<glm::mat4x4, 6>;

   private:
      six_mat4x4 m_shadowViewMatrix;

      six_mat4x4 m_shadowProjectionMatrix;

      std::shared_ptr<TextureCubeAtlasHandler> GetTextureCubeHandler() const;

   public:

      ProjectedPointShadowInfo(const LazyTextureAtlasObtainer& shadowAtlasCellResource);

      virtual ~ProjectedPointShadowInfo();

      virtual void BindShadowFramebuffer(bool clearDepthBuffer) const override;

      six_mat4x4 GetShadowViewMatrices() const;

      six_mat4x4 GetShadowProjectionMatrices() const;

      void SetShadowViewMatrices(const six_mat4x4& shadowViewMatrices);

      void SetShadowProjectionMatrix(const six_mat4x4& shadowProjectionMatrices);

      six_mat4x4 GetShadowMatrix() const;

   };
}
