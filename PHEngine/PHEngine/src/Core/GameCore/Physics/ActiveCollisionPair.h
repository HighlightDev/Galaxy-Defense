#pragma once

#include "PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/ITickable.h"

#include <stdint.h>
#include <cstdint>

namespace EnginePhysics
{
    class ActiveCollisionPair
    : public ITickable
    {
        const PhysicsDescriptor *mFirstCollisionBody;
        const PhysicsDescriptor *mSecondCollisionBody;

        uint32_t mFirstCollisionBodyId;
        uint32_t mSecondCollisionBodyId;

        float mActiveCollisionLefitime = 0.0f;
        float mActiveCollisionTimeout = 0.2f;

        bool mIsCollisionExpired{false};

        public:

        explicit ActiveCollisionPair(const PhysicsDescriptor *collisionBody1, const PhysicsDescriptor *collisionBody2);

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override {};

        uint32_t GetFirstCollisionBodyId() const;
        uint32_t GetSecondCollisionBodyId() const;

        const PhysicsDescriptor* GetFirstCollisionBody() const;
        const PhysicsDescriptor* GetSecondCollisionBody() const;

        void ReloadActiveCollisionLifetime();

        float GetActiveCollisionLifetime() const;
        float GetActiveCollisionTimeout() const;

        void SetActiveCollisionLifetime(const float lifetime);
        void SetActiveCollisionTimeout(const float timeout);

        bool IsCollisionExpired() const;
    };
}