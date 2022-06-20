#pragma once

#include <memory>

namespace EngineCore
{
    class Actor;
    class UiComponent;
}

namespace Game
{
    class CombatEntity
    {
        float mDamageDeltaTime;

        bool mIsDamageReceived;

        size_t mLifePoints;

        std::shared_ptr<::EngineCore::Actor> mSpaceShipActor;

        std::shared_ptr<::EngineCore::UiComponent> mUiComponent;

        size_t mDmgTextFieldId;

    public:
        CombatEntity(const std::shared_ptr<::EngineCore::Actor> &spaceShipActor,
                     const std::shared_ptr<::EngineCore::UiComponent> &uiComponent,
                     const size_t dmgTextFieldId);

        const std::shared_ptr<::EngineCore::Actor> &GetSpaceShipActor() const;

        bool CheckIsAliveAfterDamage(const size_t dmg);

        void SetDamageDeltaTime(const float deltaTime);

        float GetDamageDeltaTime() const;

        void SetIsDamageReceived(const bool isDamageReceived);

        bool GetIsDamageReceived() const;

        void RestoreLife();

        const std::shared_ptr<::EngineCore::UiComponent>& GetSpaceShipUiComponent() const;

        size_t GetDmgTextFieldId() const;
    };

}
