#pragma once

#include "Core/GameCore/FramebufferImplementation/ShadowFramebuffer.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasFactory.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasSpaceRequest.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <memory>

using namespace Graphics::Texture;
using namespace EngineCore::FramebufferImpl;

namespace Graphics {

enum class LightType { UNDEFINED, DIRECTIONAL_LIGHT, POINT_LIGHT, SPOT_LIGHT };

class ProjectedShadowInfo : public std::enable_shared_from_this<ProjectedShadowInfo> {
protected:
    LightType m_lightType;

    TextureAtlasSpaceRequest mShadowmapAtlasRequest;

    mutable std::shared_ptr<ShadowFramebuffer> m_shadowFramebuffer;

    glm::mat4 m_shadowBiasMatrix;

    bool bShadowmapDirty;

    glm::vec3 mPlayerPositionOffset;

    std::shared_ptr<TextureAtlasHandler> mShadowmapHandler;

    std::function <void()> mOnShadowMapUpdatedCallback;

public:
    ProjectedShadowInfo(const TextureAtlasSpaceRequest& shadowmapAtlasRequest);

    virtual ~ProjectedShadowInfo();

    virtual void Initialize();

    virtual void BindShadowFramebuffer(bool bBindFramebuffer, bool clearDepthBuffer) const;

    void SetIsShadowMapDirty(const bool bDirty);

    void SetPlayerPositionOffset(const glm::vec3& offset);

    std::shared_ptr<ITexture> GetAtlasResource() const;

    LightType GetLightType() const;

    std::string GetLightTypeAsString() const;

    glm::vec3 GetPlayerPositionOffset() const;

    bool IsShadowMapDirty() const;

    TextureAtlasSpaceRequest GetTextureAtlasSpaceRequest() const;

    void SetOnShadowMapUpdatedCallback(const std::function<void()>& callback);

    void CleanUp();

protected:
    void AllocateFramebuffer() const;

    void DeallocateFramebuffer() const;
};

} // namespace Graphics
