#include "DeferredShadingGBuffer.h"

#include "ActiveBindedState.h"

namespace Graphics {
// Buffer should be recreated when window size was changed
DeferredShadingGBuffer::DeferredShadingGBuffer(const ViewPortInfo& viewPortInfo)
    : FramebufferBundle()
    , mViewPortInfo(viewPortInfo)
    , mFramebuffer(std::make_shared<FramebufferObject>())
{
    Init();
}

DeferredShadingGBuffer::~DeferredShadingGBuffer()
{
}

void DeferredShadingGBuffer::SetTextures()
{
    AllocateTextures();

    mFramebuffer->AddRenderTexture(GL_DEPTH_STENCIL_ATTACHMENT, m_depthBuffer);
    mFramebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT0, m_positionBuffer);
    mFramebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT1, m_normalBuffer);
    mFramebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT2, m_albedoBuffer);
    mFramebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT3, m_metallicRoughnessBuffer);
    mFramebuffer->AddRenderTexture(GL_COLOR_ATTACHMENT4, m_emissionBuffer);
}

void DeferredShadingGBuffer::SetFramebuffers()
{
    mFramebuffer->CreateFramebuffer();
}

void DeferredShadingGBuffer::SetRenderbuffers()
{
}

void DeferredShadingGBuffer::CleanUp()
{
    DestroyGBuffer();
}

void DeferredShadingGBuffer::DestroyGBuffer()
{
    mFramebuffer->UnbindFramebuffer();
    mFramebuffer->CleanUp();
    TryToFreeRenderTargetTextures();
}

void DeferredShadingGBuffer::TryToFreeRenderTargetTextures()
{
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_depthBuffer);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_positionBuffer);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_normalBuffer);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_albedoBuffer);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_metallicRoughnessBuffer);
    RenderTargetPool::GetInstance()->TryToFreeMemory(m_emissionBuffer);
}

void DeferredShadingGBuffer::AllocateTextures()
{
    // Create all empty textures and attach them to gBuffer FramebufferBundle Object

    // Depth texture
    {
        TexParams depthParams(
            mViewPortInfo.Width,
            mViewPortInfo.Height,
            GL_TEXTURE_2D,
            GL_NEAREST,
            GL_NEAREST,
            0,
            GL_DEPTH24_STENCIL8,
            GL_DEPTH_STENCIL,
            GL_UNSIGNED_INT_24_8,
            GL_REPEAT,
            true);
        m_depthBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(depthParams);
    }

    // Position texture
    {
        TexParams positionParams(
            mViewPortInfo.Width,
            mViewPortInfo.Height,
            GL_TEXTURE_2D,
            GL_NEAREST,
            GL_NEAREST,
            0,
            GL_RGB16F,
            GL_RGB,
            GL_FLOAT,
            GL_REPEAT,
            true);
        m_positionBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(positionParams);
    }

    // Normal texture
    {
        TexParams normalParams(
            mViewPortInfo.Width,
            mViewPortInfo.Height,
            GL_TEXTURE_2D,
            GL_NEAREST,
            GL_NEAREST,
            0,
            GL_RGB16F,
            GL_RGB,
            GL_FLOAT,
            GL_REPEAT,
            true);
        m_normalBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(normalParams);
    }

    // Albedo component texture
    {
        TexParams aldbedoParams(
            mViewPortInfo.Width,
            mViewPortInfo.Height,
            GL_TEXTURE_2D,
            GL_NEAREST,
            GL_NEAREST,
            0,
            GL_RGB,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            GL_REPEAT,
            true);
        m_albedoBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(aldbedoParams);
    }

    // Metallic with roughness component texture
    {
        TexParams metalllicRoughnessParams(
            mViewPortInfo.Width,
            mViewPortInfo.Height,
            GL_TEXTURE_2D,
            GL_NEAREST,
            GL_NEAREST,
            0,
            GL_RG,
            GL_RG,
            GL_UNSIGNED_BYTE,
            GL_REPEAT,
            true);
        m_metallicRoughnessBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(metalllicRoughnessParams);
    }

    // Emission texture
    {
        TexParams emissionParams(
            mViewPortInfo.Width,
            mViewPortInfo.Height,
            GL_TEXTURE_2D,
            GL_NEAREST,
            GL_NEAREST,
            0,
            GL_RGBA,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            GL_REPEAT,
            true);
        m_emissionBuffer = RenderTargetPool::GetInstance()->GetOrAllocateResource<Texture2d>(emissionParams);
    }
}

void DeferredShadingGBuffer::BindDeferredGBuffer(const GLbitfield clearBufferBit)
{
    RenderToFBO(*mFramebuffer, true, mViewPortInfo, clearBufferBit);
}

void DeferredShadingGBuffer::UnbindDeferredGBuffer(const GLbitfield clearBufferBit)
{
    UnbindFramebuffer(clearBufferBit);
}

int32_t DeferredShadingGBuffer::BindRenderTarget(ActiveBindedState& activeBindedState, const RenderTarget& renderTarget)
{
    const auto occupiedSlot = activeBindedState.OccupyTextureSlot(renderTarget->GetTextureDescriptor());
    if (!occupiedSlot.bWasAlreadyBound) {
        renderTarget->BindTexture(occupiedSlot.SlotIndex);
    }
    return occupiedSlot.SlotIndex;
}

int32_t DeferredShadingGBuffer::BindDepthTexture(ActiveBindedState& activeBindedState)
{
    return BindRenderTarget(activeBindedState, m_depthBuffer);
}

int32_t DeferredShadingGBuffer::BindPositionTexture(ActiveBindedState& activeBindedState)
{
    return BindRenderTarget(activeBindedState, m_positionBuffer);
}

int32_t DeferredShadingGBuffer::BindNormalTexture(ActiveBindedState& activeBindedState)
{
    return BindRenderTarget(activeBindedState, m_normalBuffer);
}

int32_t DeferredShadingGBuffer::BindAlbedoTexture(ActiveBindedState& activeBindedState)
{
    return BindRenderTarget(activeBindedState, m_albedoBuffer);
}

int32_t DeferredShadingGBuffer::BindMetallicRoughnessTexture(ActiveBindedState& activeBindedState)
{
    return BindRenderTarget(activeBindedState, m_metallicRoughnessBuffer);
}

int32_t DeferredShadingGBuffer::BindEmissionTexture(ActiveBindedState& activeBindedState)
{
    return BindRenderTarget(activeBindedState, m_emissionBuffer);
}

void DeferredShadingGBuffer::CopyFramebufferDataToDefaultFramebuffer(
    const size_t srcX,
    const size_t srcY,
    const size_t srcResolutionX,
    const size_t srcResolutionY,
    const size_t dstX,
    const size_t dstY,
    const size_t dstResolutionX,
    const size_t dstResolutionY,
    const int32_t bufferBit)
{
    FramebufferBundle::CopySrcFramebufferDataToDefaultFramebufferImpl(
        mFramebuffer, srcX, srcY, srcResolutionX, srcResolutionY, dstX, dstY, dstResolutionX, dstResolutionY, bufferBit);
}

void DeferredShadingGBuffer::CopyFramebufferDataToDstFramebuffer(
    const std::shared_ptr<IFramebufferObject>& framebufferObjectInstance,
    const size_t srcX,
    const size_t srcY,
    const size_t srcResolutionX,
    const size_t srcResolutionY,
    const size_t dstX,
    const size_t dstY,
    const size_t dstResolutionX,
    const size_t dstResolutionY,
    const int32_t bufferBit)
{
    FramebufferBundle::CopySrcFramebufferDataToDstFramebufferImpl(
        mFramebuffer,
        framebufferObjectInstance,
        srcX,
        srcY,
        srcResolutionX,
        srcResolutionY,
        dstX,
        dstY,
        dstResolutionX,
        dstResolutionY,
        bufferBit);
}

void DeferredShadingGBuffer::ResizeRenderTargets(const ViewPortInfo& viewPortInfo)
{
    mViewPortInfo = viewPortInfo;

    mFramebuffer->UnbindFramebuffer();
    TryToFreeRenderTargetTextures();

    AllocateTextures();
    mFramebuffer->ReassignRenderTexture(GL_DEPTH_STENCIL_ATTACHMENT, m_depthBuffer);
    mFramebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT0, m_positionBuffer);
    mFramebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT1, m_normalBuffer);
    mFramebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT2, m_albedoBuffer);
    mFramebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT3, m_metallicRoughnessBuffer);
    mFramebuffer->ReassignRenderTexture(GL_COLOR_ATTACHMENT4, m_emissionBuffer);

    mFramebuffer->RebindFramebufferTextures();
}

std::shared_ptr<IFramebufferObject> DeferredShadingGBuffer::GetFramebufferObjectInstance() const
{
    return mFramebuffer;
}
} // namespace Graphics