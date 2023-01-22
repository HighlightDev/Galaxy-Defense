#pragma once

#include "SpaceObjectActor.h"
#include "Implementation/Modifiers/ModifiersHandler.h"

#include <functional>
#include <unordered_map>
#include <memory>

using namespace EngineCore;

namespace Game
{
    class AsteroidActor
        : public SpaceObjectActor
    {
        std::unique_ptr<ModifiersHandler> mModifiersHandler;

    public:
        AsteroidActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        void Tick(const float deltaTime) override;

        void TriggerSpawn(const glm::vec3 &position) override;

        void TriggerDisabled() override;

        void AddModifier(const std::shared_ptr<IModifiable> &modifier);

        bool HasModifier(const eModifierType modifierType, const uint64_t creatorObjectId) const;

        bool HasModifier(const eModifierType modifierType) const;

        std::shared_ptr<IModifiable> GetModifier(const eModifierType modifierType) const;

        void RemoveModifier(const eModifierType modifierType, const uint64_t creatorObjectId);
    };
}