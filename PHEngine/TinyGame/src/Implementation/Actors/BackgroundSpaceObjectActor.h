#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/ICameraTransformChangeNotifyable.h"
#include "Core/GameCore/ACamera.h"

using namespace EngineCore;

namespace Game
{
    class BackgroundSpaceObjectActor
        : public SpaceObjectActor
    {
        std::shared_ptr<BillboardComponent> mBackgroundSpaceObjectBillboard;

    public:
        BackgroundSpaceObjectActor(const std::string &gameObjectName, const std::shared_ptr<EngineCore::SceneComponent> &rootComponent);

        virtual void PostLevelInit() override;

        virtual void TriggerSpawn(const glm::vec3 &position) override;

        virtual void TriggerDisabled() override;
    };
}