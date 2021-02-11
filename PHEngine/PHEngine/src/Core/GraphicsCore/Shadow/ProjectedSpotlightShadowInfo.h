#pragma once
#include "ProjectedShadowInfo.h"
#include "Core/GameCore/Event/TextureAtlasGeneratedEvent.h"

namespace Graphics
{

   class ProjectedSpotlightShadowInfo
      : public ProjectedShadowInfo
      , public Event::TextureAtlasGeneratedEvent
   {
 
      glm::mat4x4 mShadowViewMatrix;

      glm::mat4x4 mShadowProjectionMatrix;

   private:

      std::shared_ptr<Texture2dAtlasHandler> GetTexture2dHandler() const;

   public:

      ProjectedSpotlightShadowInfo(const TextureAtlasSpaceRequest& shadowAtlasCellResource);

      virtual ~ProjectedSpotlightShadowInfo();

      virtual void BindShadowFramebuffer(bool bBindFramebuffer, bool clearDepthBuffer) const override;

      virtual void ProcessEvent(typename const Event::TextureAtlasGeneratedEvent::EventData_t& data) override;

      glm::mat4x4 GetShadowViewMatrix() const;

      glm::mat4x4 GetShadowProjectionMatrix() const;

      void SetShadowViewMatrix(const glm::mat4x4& shadowViewMatrix);

      void SetShadowProjectionMatrix(const glm::mat4x4& shadowProjectionMatrix);

      glm::mat4x4 GetShadowMatrix() const;

      glm::vec4 GetTextureAtlasOffset() const;

   };
}
