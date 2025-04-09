#pragma once

#include <stdint.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace Graphics {

class ActiveBindedState {

    std::string mActiveShaderName;

    std::vector<std::pair<uint32_t /*texture id*/, int32_t /*texture slot*/>> mActiveTextures;

    int32_t mAvailableTextureSlotsCount{0};

    int32_t mActiveTexturePointer{0};

public:
    ActiveBindedState();

    const std::vector<std::pair<uint32_t, int32_t>>& GetActiveTextures() const;

    const std::string& GetActiveShaderName() const;

    bool TryUpdateActiveShaderName(const std::string& activeShaderName);

    int32_t GetBindedSlotIndexByTextureId(const uint32_t textureId) const;

    int32_t OccupyTextureSlot(const uint32_t textureId);

    void Reset();
};
} // namespace Graphics
