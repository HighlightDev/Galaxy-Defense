#pragma once
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlas.h"
#include "Core/GraphicsCore/TextureAtlas/TextureAtlasSpaceRequest.h"

#include <glm/vec2.hpp>

#include <array>
#include <cstddef>
#include <memory>
#include <unordered_map>
#include <vector>

using namespace Graphics::Texture;

namespace Graphics {
class TextureAtlasFactory {

    static std::unique_ptr<TextureAtlasFactory> m_instance;

    std::vector<std::shared_ptr<TextureAtlas>> m_textureAtlases;

    std::unordered_map<size_t /*request id*/, std::shared_ptr<TextureAtlasHandler>> mTextureAtlasHandlers;

    std::array<std::vector<std::pair<size_t, glm::ivec2>>, 2> Reservations;

    std::vector<std::pair<size_t, glm::ivec2>> CubemapReservations;
    
    int32_t mShadowMapSize;

public:

    TextureAtlasFactory();

    ~TextureAtlasFactory();

    static std::unique_ptr<TextureAtlasFactory>& GetInstance()
    {
        if (!m_instance)
            m_instance = std::make_unique<TextureAtlasFactory>();

        return m_instance;
    }

    void AllocateAtlasSpace();

    TextureAtlasSpaceRequest AddTextureAtlasRequest(const eShadowMapReservationType reservationType, const glm::ivec2& size);

    TextureAtlasSpaceRequest AddTextureCubeAtlasRequest(const glm::ivec2& size);

    std::shared_ptr<TextureAtlasHandler> GetTextureAtlasCellByRequestId(size_t requestId) const;

    void DeallocateTextureAtlasByRequestId(size_t requestId);

    int32_t GetShadowMapSize() const;

private:
    void AllocateTexture2dAtlasSpace();

    void AllocateTextureCubeSpace();

    void
    AddTextureAtlasReservation(const eShadowMapReservationType reservationType, const size_t requestId, const glm::ivec2& size);

    void AddTextureCubeAtlasReservation(size_t requestId, const glm::ivec2& size);

    void SplitChunk(
        std::vector<TextureAtlasCell>& emptyChunks,
        std::vector<TextureAtlasCell>::const_iterator splittingEmptyChunkIt,
        TextureAtlasCell& splitCenterCell);
};

} // namespace Graphics
