#pragma once
#include "ProjectedShadowInfo.h"
#include "Core/GameCore/Event/TextureAtlasGeneratedEvent.h"

namespace Graphics
{

   class ProjectedDirectionalLightShadowInfo 
      : public ProjectedShadowInfo
      , public Event::TextureAtlasGeneratedGameThreadEvent
   {
      glm::mat4x4 m_shadowViewMatrix;

      glm::mat4x4 m_shadowProjectionMatrix;

      const float mShadowOrthoHalfExtent;

   private:

      std::shared_ptr<Texture2dAtlasHandler> GetTexture2dHandler() const;

   public:      

      ProjectedDirectionalLightShadowInfo(const TextureAtlasSpaceRequest& shadowAtlasCellResource, const float shadowOrthoHalfExtent);

      ~ProjectedDirectionalLightShadowInfo() override;

      void Initialize() override;

      void BindShadowFramebuffer(bool bBindFramebuffer, bool clearDepthBuffer) const override;

      void ProcessEvent(const TextureAtlasGeneratedGameThreadEvent* sender, const typename Event::TextureAtlasGeneratedGameThreadEvent::EventData_t& data) override;

      glm::mat4 GetShadowViewMatrix() const;

      glm::mat4 GetShadowProjectionMatrix() const;

      void SetShadowViewMatrix(const glm::mat4& shadowViewMatrix);

      void SetShadowProjectionMatrix(const glm::mat4& shadowProjectionMatrix);

      glm::mat4 GetShadowMatrix() const;

      glm::vec4 GetTextureAtlasOffset() const;

      float GetShadowOrthoHalfExtent() const {
         return mShadowOrthoHalfExtent;
      }
   };

}

