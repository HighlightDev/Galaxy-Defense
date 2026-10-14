#pragma once

#include "Core/GraphicsCore/TextureAtlas/TextureAtlasSpaceRequest.h"

#include <memory>

namespace Graphics {
namespace Data {
struct CubemapRenderData {
    TextureAtlasSpaceRequest m_texture;

    CubemapRenderData(TextureAtlasSpaceRequest texture)
        : m_texture(texture)
    {
    }
};
} // namespace Data
} // namespace Graphics
