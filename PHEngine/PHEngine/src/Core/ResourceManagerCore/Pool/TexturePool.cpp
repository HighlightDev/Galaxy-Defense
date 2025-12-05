#include "TexturePool.h"

#include "Core/CommonCore/Assertion.h"

namespace Resources {
std::shared_ptr<TexturePool> TexturePool::m_instance;

std::shared_ptr<ITexture> TexturePool::GetTextureAt(const size_t index) const
{
    ext_assert(index < resourceMap.size(), "TexturePool::GetTextureAt: index out of range");
    resourceMap_t::const_iterator startIt = resourceMap.begin();
    std::advance(startIt, index);
    return startIt->second;
}

size_t TexturePool::GetTexturesCount() const
{
    return GetResourcesCount();
}

std::string TexturePool::ToString() const
{
    return "TexturePool";
}

std::string TexturePool::GetTextureName(const std::shared_ptr<ITexture>& texture) const
{
    const auto texParamOpt = GetKeyOptional(texture);
    if (texParamOpt.has_value()) {
        return texParamOpt.value();
    }
    return "Unknown";
}

std::shared_ptr<TexturePool> TexturePool::GetInstance()
{
    if (!m_instance)
        m_instance = std::make_shared<TexturePool>();

    return m_instance;
}

void TexturePool::ReloadInstance()
{
    if (m_instance)
        m_instance.reset();
}
} // namespace Resources