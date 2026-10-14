#pragma once

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Policy/Policies.h"
#include "TEvent.h"

using namespace Graphics::Texture;

namespace Event {
class TextureAtlasGeneratedGameThreadEvent
    : public TEvent<TextureAtlasGeneratedGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eTextureType>> {
public:
    using Type_t
        = TEvent<TextureAtlasGeneratedGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eTextureType>>::
            Type_t;

    std::string ToString() const override
    {
        return "TextureAtlasGeneratedGameThreadEvent";
    }
};

} // namespace Event
