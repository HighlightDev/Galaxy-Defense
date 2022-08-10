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

enum class eSpaceshipActivityState
{
    IDLE,
    ACTIVE,
};

namespace Game
{
    class SpaceshipActor
        : public Actor
    {
    protected:
        eSpaceshipActivityState mActivityState{eSpaceshipActivityState::IDLE};

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

        virtual void TriggerDamageReceived(const size_t damage);

        virtual void TriggerSpawn(const glm::vec3 &position);

        virtual void TriggerExplosion();

        virtual void TriggerDisable();

        void AddModifier(const std::shared_ptr<IModifiable> &modifier);

        bool CheckIsAliveAfterDamage(const size_t dmg);

        void SetDamageDeltaTime(const float deltaTime);

        float GetDamageDeltaTime() const;

        void SetIsDamageReceived(const bool isDamageReceived);

        bool GetIsDamageReceived() const;

        void RestoreLife();

        const std::shared_ptr<TextField> &GetDamageFieldText() const;

        eSpaceshipActivityState GetSpaceshipActivityState() const;
        
        protected:

        virtual glm::vec2 CalculatePositionForDamageText() const;
    };
}