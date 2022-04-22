#pragma once

#include <memory>

namespace EngineCore
{
    class Actor;
}

namespace Game
{
    class CombatEntity
    {
        float mDamageDeltaTime;

        bool mIsDamageReceived;

        size_t mLifePoints;

        std::shared_ptr<::EngineCore::Actor> mSpaceShipActor;

    public:
        CombatEntity(const std::shared_ptr<::EngineCore::Actor> &spaceShipActor);

        const std::shared_ptr<::EngineCore::Actor> &GetSpaceShipActor() const;

        bool CheckIsAliveAfterDamage(const float dmg);

        void SetDamageDeltaTime(const float deltaTime);

        float GetDamageDeltaTime() const;

        void SetIsDamageReceived(const bool isDamageReceived);

        bool GetIsDamageReceived() const;

        void RestoreLife();
    };

}

