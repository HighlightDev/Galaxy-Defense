#pragma once

#include "Implementation/Modifiers/IModifiable.h"
#include "Core/GameCore/ITickable.h"

#include <memory>
#include <vector>

namespace Game
{
    class ModifiersHandler
        : public ITickable
    {
    protected:
        std::vector<std::shared_ptr<IModifiable>> mModifiers;

    public:
        ModifiersHandler();

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override;

        void RemoveAllModifiers();

        void AddModifier(const std::shared_ptr<IModifiable> &modifier);

        bool HasModifier(const eModifierType modifierType, const uint64_t creatorObjectId) const;

        bool HasModifier(const eModifierType modifierType) const;

        std::shared_ptr<IModifiable> GetModifier(const eModifierType modifierType) const;

        void RemoveModifier(const eModifierType modifierType, const uint64_t creatorObjectId);

    private:
        void RemoveExpiredModifiers();
    };
}