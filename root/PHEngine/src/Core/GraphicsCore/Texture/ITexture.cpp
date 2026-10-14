#include "ITexture.h"

namespace Graphics {
namespace Texture {
ITexture::ITexture(const std::string& textureName)
    : m_texDescriptor(-1)
    , m_textureName(textureName)
{
}

bool ITexture::operator==(const ITexture& right) const
{
    return this->m_texDescriptor == right.m_texDescriptor;
}

std::string ITexture::GetTextureName() const
{
    return m_textureName;
}

} // namespace Texture
} // namespace Graphics