#pragma once

#include "PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/ITickable.h"

#include <stdint.h>
#include <cstdint>
#include <memory>

namespace EnginePhysics
{
    class ActiveCollisionPair
        : public ITickable
    {
        std::weak_ptr<PhysicsDescriptor> mFirstCollisionBody;
        std::weak_ptr<PhysicsDescriptor> mSecondCollisionBody;

        uint32_t mFirstCollisionBodyId;
        uint32_t mSecondCollisionBodyId;

        ePhysicsBodyType mFirstCollisionBodyType;
        ePhysicsBodyType mSecondCollisionBodyType;

        int32_t mFirstCollisionBodyOwnerActorObjectId;
        int32_t mSecondCollisionBodyOwnerActorObjectId;

        float mActiveCollisionLefitime = 0.0f;
        float mActiveCollisionTimeout = 0.2f;

        bool mIsCollisionExpired{false};

    public:
        explicit ActiveCollisionPair(const std::shared_ptr<PhysicsDescriptor> &collisionBody1, const std::shared_ptr<PhysicsDescriptor> &collisionBody2);

        void Tick(const float deltaTime) override;

        void UnpausableTick(const float deltaTime) override{};

        uint32_t GetFirstCollisionBodyId() const;
        uint32_t GetSecondCollisionBodyId() const;

        ePhysicsBodyType GetFirstCollisionBodyType() const;
        ePhysicsBodyType GetSecondCollisionBodyType() const;

        int32_t GetFirstCollisionBodyOwnerActorObjectId() const;
        int32_t GetSecondCollisionBodyOwnerActorObjectId() const;

        std::weak_ptr<PhysicsDescriptor> GetFirstCollisionBody() const;
        std::weak_ptr<PhysicsDescriptor> GetSecondCollisionBody() const;

        void ReloadActiveCollisionLifetime();

        float GetActiveCollisionLifetime() const;
        float GetActiveCollisionTimeout() const;

        void SetActiveCollisionLifetime(const float lifetime);
        void SetActiveCollisionTimeout(const float timeout);

        bool IsCollisionExpired() const;
    };
}