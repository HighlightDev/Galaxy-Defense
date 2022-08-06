#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/BoundingBox.h"
#include "Implementation/Modifiers/IModifiable.h"

#include <functional>
#include <unordered_map>
#include <memory>

using namespace EngineCore;

namespace EngineCore
{
    class TextField;
}

namespace Game
{
    class SpaceshipActor
        : public Actor
    {
    protected:

        std::vector<std::shared_ptr<IModifiable>> mModifiers;

        size_t mLifePoints;

        std::shared_ptr<TextField> mDamageTextField;

        float mDamageEffectTimePassed;

        float mDamageEffectDuration;

        float mDamageTextShowDuration;

        float mDamageTextTimePassed;

        bool mIsDamageEffectActive;

        bool mIsDamageTextActive;

    public:
        SpaceshipActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        virtual void PostLevelInit() override;

        virtual void Tick(const float deltaTime) override;

        virtual bool IsInsideLevel(const BoundingBox &boundingBox) const;

        void AddModifier(const std::shared_ptr<IModifiable>& modifier);

        void TriggerDamageReceived(const size_t damage);

        bool CheckIsAliveAfterDamage(const size_t dmg);

        void SetDamageDeltaTime(const float deltaTime);

        float GetDamageDeltaTime() const;

        void SetIsDamageReceived(const bool isDamageReceived);

        bool GetIsDamageReceived() const;

        void RestoreLife();

        const std::shared_ptr<TextField>& GetDamageFieldText() const;
    };
}