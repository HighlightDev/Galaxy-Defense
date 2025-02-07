#pragma once

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Implementation/Actors/SpaceObjectActor.h"
#include "Implementation/ICameraTransformChangeNotifyable.h"

using namespace EngineCore;

namespace Game {
class BackgroundSpaceObjectActor : public SpaceObjectActor {
    std::shared_ptr<BillboardComponent> mBackgroundSpaceObjectBillboard;

public:
    BackgroundSpaceObjectActor(
        const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void PostLevelInit() override;

    void TriggerSpawn(const glm::vec3& position) override;

    void TriggerDisabled() override;

    void SetBillboardExtentSize(const float size);
};
} // namespace Game