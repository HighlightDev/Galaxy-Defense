#pragma once
#include "ProjectedShadowInfo.h"
#include "Core/GameCore/Event/TextureAtlasGeneratedEvent.h"

#include <array>

namespace Graphics
{

   class ProjectedPointLightShadowInfo
      : public ProjectedShadowInfo
      , public Event::TextureAtlasGeneratedEvent
   {
   public:

      using six_mat4x4 = std::array<glm::mat4x4, 6>;

   private:

      six_mat4x4 m_shadowViewMatrix;

      six_mat4x4 m_shadowProjectionMatrix;

   private:

      std::shared_ptr<TextureCubeAtlasHandler> GetTextureCubeHandler() const;

   public:

      ProjectedPointLightShadowInfo(const TextureAtlasSpaceRequest& shadowAtlasCellResource);

      ~ProjectedPointLightShadowInfo() override;

      void Initialize() override;

      void BindShadowFramebuffer(bool bBindFramebuffer, bool clearDepthBuffer) const override;

      void ProcessEvent(const typename Event::TextureAtlasGeneratedEvent::EventData_t& data) override;

      six_mat4x4 GetShadowViewMatrices() const;

      six_mat4x4 GetShadowProjectionMatrices() const;

      void SetShadowViewMatrices(const six_mat4x4& shadowViewMatrices);

      void SetShadowProjectionMatrix(const six_mat4x4& shadowProjectionMatrices);

      six_mat4x4 GetShadowMatrix() const;

   };
}
