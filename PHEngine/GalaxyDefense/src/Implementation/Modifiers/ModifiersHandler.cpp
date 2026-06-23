#include "ModifiersHandler.h"

#include <algorithm>

namespace Game {
ModifiersHandler::ModifiersHandler()
    : mModifiers()
{
}

void ModifiersHandler::RemoveAllModifiers()
{
    std::for_each(mModifiers.begin(), mModifiers.end(), [](const auto& modifier) { modifier->OnPreRemoved(); });

    mModifiers.clear();
}

void ModifiersHandler::RemoveExpiredModifiers()
{
    const auto expiredIt = std::remove_if(mModifiers.begin(), mModifiers.end(), [](const auto& modifier) {
        const bool isExpired = modifier->IsExpired();
        if (isExpired) {
            modifier->OnPreRemoved();
        }
        return isExpired;
    });

    if (mModifiers.end() != expiredIt) {
        mModifiers.erase(expiredIt, mModifiers.end());
    }
}

void ModifiersHandler::Tick(const float deltaTimeSec, const float playSpeed)
{
    RemoveExpiredModifiers();

    for (const auto& modifier : mModifiers) {
        modifier->Tick(deltaTimeSec, playSpeed);
    }
}

void ModifiersHandler::UnpausableTick(const float deltaTimeSec, const float playSpeed)
{
}

void ModifiersHandler::AddModifier(const std::shared_ptr<IModifiable>& modifier)
{
    mModifiers.emplace_back(modifier);
}

bool ModifiersHandler::HasModifier(const eModifierType modifierType, const int32_t creatorObjectId) const
{
    auto foundIt = std::find_if(mModifiers.begin(), mModifiers.end(), [=](const auto& modifier) {
        return (modifierType == modifier->GetModifierType() && creatorObjectId == modifier->CreatorObjectId());
    });
    return mModifiers.end() != foundIt;
}

bool ModifiersHandler::HasModifier(const eModifierType modifierType) const
{
    auto foundIt = std::find_if(
        mModifiers.begin(), mModifiers.end(), [=](const auto& modifier) { return modifierType == modifier->GetModifierType(); });
    return mModifiers.end() != foundIt;
}

std::shared_ptr<IModifiable> ModifiersHandler::GetModifier(const eModifierType modifierType) const
{
    auto foundIt = std::find_if(
        mModifiers.begin(), mModifiers.end(), [=](const auto& modifier) { return modifierType == modifier->GetModifierType(); });
    return mModifiers.end() != foundIt ? *foundIt : nullptr;
}

void ModifiersHandler::RemoveModifier(const eModifierType modifierType, const int32_t creatorObjectId)
{
    const auto matches = [=](const auto& modifier) {
        return (modifierType == modifier->GetModifierType() && creatorObjectId == modifier->CreatorObjectId());
    };

    for (const auto& modifier : mModifiers) {
        if (matches(modifier)) {
            modifier->OnPreRemoved();
        }
    }

    const auto removeIt = std::remove_if(mModifiers.begin(), mModifiers.end(), matches);
    mModifiers.erase(removeIt, mModifiers.end());
}
} // namespace Game