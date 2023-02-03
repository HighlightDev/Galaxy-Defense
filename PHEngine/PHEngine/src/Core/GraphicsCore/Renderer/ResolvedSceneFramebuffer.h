#pragma once
#include "Core/ResourceManagerCore/Pool/RenderTargetPool.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferBundle.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GraphicsCore/OpenGL/Framebuffer/FramebufferObject.h"

using namespace Graphics::Texture;
using namespace Resources;

namespace Graphics
{
    class ResolvedSceneFramebuffer
        : public FramebufferBundle
    {
        using RenderTarget = std::shared_ptr<ITexture>;

        const ViewPortInfo mViewPortInfo;

        RenderTarget m_resolvedSceneColorBuffer;

        std::shared_ptr<FramebufferObject> mFramebuffer;

    public:
        ResolvedSceneFramebuffer(const ViewPortInfo &viewPortInfo);

        ~ResolvedSceneFramebuffer() override;

        void SetTextures() override;

        void SetFramebuffers() override;

        void SetRenderbuffers() override;

        void CleanUp() override;

        void BindResolvedSceneFramebuffer(const int32_t clearBufferBit);

        void UnbindResolvedSceneFramebuffer();

        void BindResolvedSceneColorTexture(int32_t slot);

        std::shared_ptr<ITexture> GetResolvedSceneColorTexture() const;

        std::shared_ptr<IFramebufferObject> GetFramebufferObjectInstance() const;

        void CopyFramebufferDataToDefaultFramebuffer(const size_t srcX, const size_t srcY, const size_t srcResolutionX, const size_t srcResolutionY,
                                                     const size_t dstX, const size_t dstY, const size_t dstResolutionX, const size_t dstResolutionY,
                                                     const int32_t bufferBit) override;

        void CopyFramebufferDataToDstFramebuffer(const std::shared_ptr<IFramebufferObject> &dstFramebuffer, const size_t srcX, const size_t srcY, const size_t srcResolutionX, const size_t srcResolutionY,
                                                 const size_t dstX, const size_t dstY, const size_t dstResolutionX, const size_t dstResolutionY,
                                                 const int32_t bufferBit) override;

    private:
        void DestroyResolvedSceneFramebuffer();
    };
}
