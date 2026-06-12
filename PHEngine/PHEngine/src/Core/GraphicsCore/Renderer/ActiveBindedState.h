#pragma once

#include <stdint.h>

#include <cstdint>
#include <string>
#include <vector>

namespace Graphics {

class ActiveBindedState {

    struct TextureSlot {
        uint32_t TextureId;
        uint64_t LastUseStamp; // monotonically increasing; higher == used more recently
    };

    std::string mActiveShaderName;

    std::vector<TextureSlot> mTextureSlots; // index in the vector == GL texture slot

    int32_t mAvailableTextureSlotsCount{0};

    uint64_t mUseStampCounter{0};

public:
    struct OccupiedSlot {
        int32_t SlotIndex;
        bool bWasAlreadyBound; // true -> the texture already resides in SlotIndex, no GL bind call is needed
    };

    ActiveBindedState();

    const std::string& GetActiveShaderName() const;

    bool TryUpdateActiveShaderName(const std::string& activeShaderName);

    // The single entry point for texture slot assignment: first checks whether the texture is already
    // bound (callers must not pre-check it themselves); if not, places it into a free slot or evicts the
    // least-recently-used one. Always call this right before ITexture::BindTexture and bind only when
    // bWasAlreadyBound == false.
    OccupiedSlot OccupyTextureSlot(const uint32_t textureId);

    void Reset();

private:
    int32_t GetBindedSlotIndexByTextureId(const uint32_t textureId) const;
};
} // namespace Graphics
