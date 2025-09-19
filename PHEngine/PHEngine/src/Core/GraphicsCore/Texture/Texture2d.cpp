#include "Texture2d.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/CommonCore/ThreadHelper.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/RawResource.h"
#include "Core/IoCore/TextureLoaderCore/StbLoader/StbLoader.h"
#include "Core/IoCore/TextureLoaderCore/TextureResourceInfo.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"

#include <TinyLogger/LogInterface.h>

using namespace TinyLogger;
using namespace IO;

namespace Graphics {
namespace Texture {

Texture2d::Texture2d(uint32_t texDescriptor, glm::ivec2 texBufferWH)
    : ITexture()
    , m_mipmapState(nullptr)
{
    LogInfo("Texture2d::ctor(uint32_t, glm::ivec2): texDescriptor = ", texDescriptor);
    m_texDescriptor = texDescriptor;
    m_textureParams.TexBufferWidth = texBufferWH.x;
    m_textureParams.TexBufferHeight = texBufferWH.y;
}

Texture2d::Texture2d(const std::string& pathToTex, ITextureMipMapState* mipmapState)
    : ITexture()
    , m_mipmapState(mipmapState)
{
    m_texDescriptor = GetTextureResource(pathToTex);
    LogInfo("Texture2d::ctor(const std::string&, ITextureMipMapState*): texDescriptor = ", m_texDescriptor);
}

Texture2d::Texture2d(const TexParams& textureParameters)
    : ITexture()
    , m_textureParams(textureParameters)
    , m_mipmapState(nullptr)
{
    InitEmptyTexture();
    LogInfo("Texture2d::ctor(const TexParams &): texDescriptor = ", m_texDescriptor);
}

Texture2d::~Texture2d()
{
    if (m_mipmapState)
        delete m_mipmapState;
}

void Texture2d::InitEmptyTexture()
{
    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));
    glGenTextures(1, &m_texDescriptor);
    glBindTexture(GL_TEXTURE_2D, m_texDescriptor);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        m_textureParams.TexPixelInternalFormat,
        m_textureParams.TexBufferWidth,
        m_textureParams.TexBufferHeight,
        0,
        m_textureParams.TexPixelFormat,
        m_textureParams.TexPixelType,
        0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_textureParams.TexWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_textureParams.TexWrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_textureParams.TexMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_textureParams.TexMinFilter);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2d::BindTexture(uint32_t textureSlot) const
{
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(m_textureParams.TexTarget, m_texDescriptor);
}

void Texture2d::UnbindTexture(uint32_t textureSlot) const
{
    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(m_textureParams.TexTarget, 0);
}

uint32_t Texture2d::GetTextureResource(const std::string& pathToTex, int32_t texWrapMode)
{
    Resource* outResource;
    const bool bResourceValid = ResourceMap::GetInstance()->TryGetResource(outResource, pathToTex);

    ext_assert(bResourceValid, "Missing texture resource: " + pathToTex);

    TextureResource* texResource = static_cast<TextureResource*>(outResource);

    m_textureParams.TexBufferWidth = texResource->mTexInfo.Width;
    m_textureParams.TexBufferHeight = texResource->mTexInfo.Height;

    if (texResource->mTexInfo.PixelComponents == 1) {
        m_textureParams.TexPixelFormat = GL_RED;
        m_textureParams.TexPixelInternalFormat = GL_RED;
    } else if (texResource->mTexInfo.PixelComponents == 3) {
        m_textureParams.TexPixelFormat = GL_RGB;
        m_textureParams.TexPixelInternalFormat = GL_RGB;
    } else if (texResource->mTexInfo.PixelComponents == 4) {
        m_textureParams.TexPixelFormat = GL_RGBA;
        m_textureParams.TexPixelInternalFormat = GL_RGBA;
    }

    const auto resultTexture = CreateTexture(texResource->mData);
    return resultTexture;
}

uint32_t Texture2d::CreateTexture(const void* pixelsData)
{
    uint32_t texObject = -1;
    int32_t& textureTarget = m_textureParams.TexTarget;

    assert(ThreadHelper::GetInstance()->IsCurrentThreadEqualToProvidedByName("Render"));
    glGenTextures(1, &texObject);

    glBindTexture(textureTarget, texObject);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, m_textureParams.TexWrapMode);
    glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, m_textureParams.TexWrapMode);
    glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, m_textureParams.TexMagFilter);
    glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, m_textureParams.TexMinFilter);
    if (m_mipmapState) {
        m_mipmapState->ExecuteTextureSampleFilteringInstructions();
    }

    glTexImage2D(
        textureTarget,
        0,
        m_textureParams.TexPixelInternalFormat,
        m_textureParams.TexBufferWidth,
        m_textureParams.TexBufferHeight,
        0,
        m_textureParams.TexPixelFormat,
        m_textureParams.TexPixelType,
        pixelsData);

    glBindTexture(textureTarget, 0);

    return texObject;
}

void Texture2d::CleanUp()
{
    LogInfo("Texture2d::CleanUp: texDescriptor = ", m_texDescriptor);
    glDeleteTextures(1, &m_texDescriptor);
}

float Texture2d::GetTextureAspectRatio() const
{
    return (static_cast<float>(m_textureParams.TexBufferWidth) / static_cast<float>(m_textureParams.TexBufferHeight));
}

eTextureType Texture2d::GetTextureType() const
{
    return eTextureType::TEXTURE_2D;
}
} // namespace Texture
} // namespace Graphics