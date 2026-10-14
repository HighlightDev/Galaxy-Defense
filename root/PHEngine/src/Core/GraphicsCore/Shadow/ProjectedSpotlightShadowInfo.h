#pragma once
#include "Core/GameCore/Event/TextureAtlasGeneratedEvent.h"
#include "ProjectedShadowInfo.h"

namespace Graphics {

class ProjectedSpotlightShadowInfo : public ProjectedShadowInfo, public Event::TextureAtlasGeneratedGameThreadEvent {

    glm::mat4x4 mShadowViewMatrix;

    glm::mat4x4 mShadowProjectionMatrix;

private:
    std::shared_ptr<Texture2dAtlasHandler> GetTexture2dHandler() const;

public:
    ProjectedSpotlightShadowInfo(const TextureAtlasSpaceRequest& shadowAtlasCellResource);

    ~ProjectedSpotlightShadowInfo() override;

    void Initialize() override;

    void BindShadowFramebuffer(bool bBindFramebuffer, bool clearDepthBuffer) const override;

    void ProcessEvent(
        const TextureAtlasGeneratedGameThreadEvent* sender,
        const typename Event::TextureAtlasGeneratedGameThreadEvent::EventData_t& data) override;

    glm::mat4x4 GetShadowViewMatrix() const;

    glm::mat4x4 GetShadowProjectionMatrix() const;

    void SetShadowViewMatrix(const glm::mat4x4& shadowViewMatrix);

    void SetShadowProjectionMatrix(const glm::mat4x4& shadowProjectionMatrix);

    glm::mat4x4 GetShadowMatrix() const;

    glm::vec4 GetTextureAtlasOffset() const;
};
} // namespace Graphics
