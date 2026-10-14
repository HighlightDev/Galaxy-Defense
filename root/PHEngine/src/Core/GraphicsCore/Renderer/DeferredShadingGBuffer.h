#pragma once
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferObject.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"

using namespace Graphics::Texture;
using namespace Resources;

namespace Graphics {

class ActiveBindedState;

class DeferredShadingGBuffer : public FramebufferBundle {
    using RenderTarget = std::shared_ptr<ITexture>;

    ViewPortInfo mViewPortInfo;

    RenderTarget m_depthBuffer;
    RenderTarget m_positionBuffer;
    RenderTarget m_normalBuffer;
    RenderTarget m_albedoBuffer;
    RenderTarget m_metallicRoughnessBuffer;
    RenderTarget m_emissionBuffer;

    std::shared_ptr<FramebufferObject> mFramebuffer;

public:
    // Buffer should be recreated when window size was changed
    DeferredShadingGBuffer(const ViewPortInfo& viewPortInfo);

    ~DeferredShadingGBuffer() override;

    void SetTextures() override;

    void SetFramebuffers() override;

    void SetRenderbuffers() override;

    void CleanUp() override;

    void BindDeferredGBuffer(const GLbitfield clearBufferBit);

    void UnbindDeferredGBuffer(const GLbitfield clearBufferBit);

    // Each Bind*Texture occupies a slot through ActiveBindedState (binding only when not already bound)
    // and returns the slot index the caller should load into the sampler uniform.
    int32_t BindDepthTexture(ActiveBindedState& activeBindedState);

    int32_t BindPositionTexture(ActiveBindedState& activeBindedState);

    int32_t BindNormalTexture(ActiveBindedState& activeBindedState);

    int32_t BindAlbedoTexture(ActiveBindedState& activeBindedState);

    int32_t BindMetallicRoughnessTexture(ActiveBindedState& activeBindedState);

    int32_t BindEmissionTexture(ActiveBindedState& activeBindedState);

    std::shared_ptr<IFramebufferObject> GetFramebufferObjectInstance() const;

    void CopyFramebufferDataToDefaultFramebuffer(
        const size_t srcX,
        const size_t srcY,
        const size_t srcResolutionX,
        const size_t srcResolutionY,
        const size_t dstX,
        const size_t dstY,
        const size_t dstResolutionX,
        const size_t dstResolutionY,
        const int32_t bufferBit) override;

    void CopyFramebufferDataToDstFramebuffer(
        const std::shared_ptr<IFramebufferObject>& framebufferObjectInstance,
        const size_t srcX,
        const size_t srcY,
        const size_t srcResolutionX,
        const size_t srcResolutionY,
        const size_t dstX,
        const size_t dstY,
        const size_t dstResolutionX,
        const size_t dstResolutionY,
        const int32_t bufferBit) override;

    void ResizeRenderTargets(const ViewPortInfo& viewPortInfo);

private:
    int32_t BindRenderTarget(ActiveBindedState& activeBindedState, const RenderTarget& renderTarget);

    void DestroyGBuffer();

    void TryToFreeRenderTargetTextures();

    void AllocateTextures();
};
} // namespace Graphics
