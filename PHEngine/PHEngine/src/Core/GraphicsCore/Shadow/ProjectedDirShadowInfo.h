#pragma once
#include "ProjectedShadowInfo.h"
#include "Core/GameCore/Event/TextureAtlasGeneratedEvent.h"

namespace Graphics
{

   class ProjectedDirShadowInfo 
      : public ProjectedShadowInfo
      , public Event::TextureAtlasGeneratedEvent
   {
      glm::mat4x4 m_shadowViewMatrix;

      glm::mat4x4 m_shadowProjectionMatrix;

      const float mShadowOrthoHalfExtent;

   private:

      std::shared_ptr<Texture2dAtlasHandler> GetTexture2dHandler() const;

   public:      

      ProjectedDirShadowInfo(const TextureAtlasSpaceRequest& shadowAtlasCellResource, const float shadowOrthoHalfExtent);

      virtual ~ProjectedDirShadowInfo();

      virtual void BindShadowFramebuffer(bool clearDepthBuffer) const override;

      virtual void ProcessEvent(typename const Event::TextureAtlasGeneratedEvent::EventData_t& data) override;

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

