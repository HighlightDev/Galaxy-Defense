#include "ActiveBindedState.h"

#include <gl/glew.h>

#include <algorithm>

namespace Graphics {

ActiveBindedState::ActiveBindedState()
    : mActiveShaderName()
    , mActiveTextures()
{
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mAvailableTextureSlotsCount);
}

const std::vector<std::pair<uint32_t, int32_t>>& ActiveBindedState::GetActiveTextures() const
{
    return mActiveTextures;
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
        mActiveTextures.cbegin(), mActiveTextures.cend(), [textureId](const auto& pair) { return pair.first == textureId; });
    if (foundIt != mActiveTextures.cend()) {
        return foundIt->second;
    }
    return -1;
}

int32_t ActiveBindedState::OccupyTextureSlot(const uint32_t textureId)
{
    if (mActiveTextures.size() >= mAvailableTextureSlotsCount && mActiveTexturePointer >= mAvailableTextureSlotsCount) {
        mActiveTexturePointer = 0;
    }

    const auto value = std::make_pair(textureId, mActiveTexturePointer);
    if (mActiveTextures.size() <= mActiveTexturePointer) {
        mActiveTextures.emplace_back(value);
    } else {
        mActiveTextures[mActiveTexturePointer] = value;
    }

    ++mActiveTexturePointer;

    return value.second;
}

void ActiveBindedState::Reset()
{
    mActiveTexturePointer = 0;
    mActiveTextures.clear();
    mActiveShaderName.clear();
}
} // namespace Graphics
