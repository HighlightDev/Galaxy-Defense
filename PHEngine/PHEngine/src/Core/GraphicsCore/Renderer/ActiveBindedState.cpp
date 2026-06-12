#include "ActiveBindedState.h"

#include <gl/glew.h>

#include <algorithm>

namespace Graphics {

ActiveBindedState::ActiveBindedState()
    : mActiveShaderName()
    , mTextureSlots()
{
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mAvailableTextureSlotsCount);
    mTextureSlots.reserve(mAvailableTextureSlotsCount);
}

const std::string& ActiveBindedState::GetActiveShaderName() const
{
    return mActiveShaderName;
}

bool ActiveBindedState::TryUpdateActiveShaderName(const std::string& activeShaderName)
{
    if (mActiveShaderName != activeShaderName) {
        mActiveShaderName = activeShaderName;
        return true;
    }

    return false;
}

int32_t ActiveBindedState::GetBindedSlotIndexByTextureId(const uint32_t textureId) const
{
    const auto foundIt = std::find_if(
        mTextureSlots.cbegin(), mTextureSlots.cend(), [textureId](const auto& slot) { return slot.TextureId == textureId; });
    if (foundIt != mTextureSlots.cend()) {
        return static_cast<int32_t>(std::distance(mTextureSlots.cbegin(), foundIt));
    }
    return -1;
}

ActiveBindedState::OccupiedSlot ActiveBindedState::OccupyTextureSlot(const uint32_t textureId)
{
    // Already bound -> refresh its LRU stamp (so the rest of the current draw can't evict it) and reuse the slot.
    const int32_t bindedSlotIndex = GetBindedSlotIndexByTextureId(textureId);
    if (-1 != bindedSlotIndex) {
        mTextureSlots[bindedSlotIndex].LastUseStamp = ++mUseStampCounter;
        return {bindedSlotIndex, true};
    }

    // A free slot is still available -> take it.
    if (mTextureSlots.size() < static_cast<size_t>(mAvailableTextureSlotsCount)) {
        mTextureSlots.push_back({textureId, ++mUseStampCounter});
        return {static_cast<int32_t>(mTextureSlots.size()) - 1, false};
    }

    // Out of capacity -> evict the least-recently-used slot. Every texture of the current draw goes
    // through OccupyTextureSlot (a hit refreshes the stamp too), so the current draw's textures always
    // carry the highest stamps and the evicted slot is guaranteed not to be referenced by a sampler
    // uniform of the draw being assembled
    const auto lruIt = std::min_element(mTextureSlots.begin(), mTextureSlots.end(), [](const auto& left, const auto& right) {
        return left.LastUseStamp < right.LastUseStamp;
    });
    lruIt->TextureId = textureId;
    lruIt->LastUseStamp = ++mUseStampCounter;
    return {static_cast<int32_t>(std::distance(mTextureSlots.begin(), lruIt)), false};
}

void ActiveBindedState::Reset()
{
    mTextureSlots.clear();
    mActiveShaderName.clear();
}
} // namespace Graphics
