#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/GUI/UiElements/IUiTransformable.h"
#include "Core/GameCore/Tweener/ITweenStateChangeNotifyable.h"
#include "Core/GameCore/Tweener/StateProperty.h"
#include "Game/DropLootType.h"

using namespace EngineCore;

namespace EngineCore {
class SceneComponent;
class Tweener;
} // namespace EngineCore

namespace Game {

enum class eLootState { NA, IDLE, ACTIVE };

class LootActor : public Actor, public ITweenStateChangeNotifyable {

    eDropLootType mLootCategory;

    eLootState mLootState;

    std::shared_ptr<Tweener> mLootTweener;
    std::shared_ptr<StateProperty<eEnginePropertyBindingType::Vec3QuadraticBezier>> mIdleBezierProp;
    std::shared_ptr<StateProperty<eEnginePropertyBindingType::Vec3QuadraticBezier>> mCollectedBezierProp;
    std::shared_ptr<StateProperty<eEnginePropertyBindingType::EulerAnglesRotation>> mCollectedRotProp;

    std::weak_ptr<EngineCore::GUI::IUiTransformable> mCachedCrystalTile;

    std::atomic_bool bIsCollecting{false};

public:
    LootActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void CleanUp() override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

    eDropLootType GetLootCategory() const;

    void SetLootCategory(const eDropLootType lootCategory);

    void SetLootState(const eLootState lootState);

    eLootState GetLootState() const;

    virtual void CollectLoot();

    virtual void SpawnLoot(const glm::vec3& position);

    void OnTweenStateChanged(const std::string& stateName) override;

    void SetupLootTweener(
        std::shared_ptr<Tweener> tweener,
        std::shared_ptr<StateProperty<eEnginePropertyBindingType::Vec3QuadraticBezier>> idleProp,
        std::shared_ptr<StateProperty<eEnginePropertyBindingType::Vec3QuadraticBezier>> collectedProp,
        std::shared_ptr<StateProperty<eEnginePropertyBindingType::EulerAnglesRotation>> collectedRotProp);
};
} // namespace Game