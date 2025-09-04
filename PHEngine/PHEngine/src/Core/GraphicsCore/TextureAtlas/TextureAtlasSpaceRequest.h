#pragma once

#include "TextureAtlasHandler.h"

enum class eShadowMapReservationType { DIRECTIONAL_LIGHT_SHADOW_MAP = 0, SPOT_LIGHT_SHADOW_MAP = 1 };

namespace Graphics {
class TextureAtlasSpaceRequest {
    static size_t m_requestId;

public:
    size_t MyRequestId;

    TextureAtlasSpaceRequest();
};
} // namespace Graphics
