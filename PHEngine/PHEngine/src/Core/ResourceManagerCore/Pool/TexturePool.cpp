#include "TexturePool.h"

#include "Core/CommonCore/Assertion.h"

namespace Resources {
std::shared_ptr<TexturePool> TexturePool::m_instance;

std::shared_ptr<ITexture> TexturePool::GetTextureAt(const size_t index) const
{
    assert(index < resourceMap.size());
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