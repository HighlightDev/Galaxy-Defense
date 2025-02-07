#pragma once

#include "Core/GameCore/BoundingBox3D.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Implementation/Actors/ElectroRayChainActor.h"
#include "Implementation/Actors/MissileActor.h"

#include <glm/vec3.hpp>

#include <functional>
#include <memory>
#include <unordered_map>

using namespace EngineCore;

namespace EngineCore {
class RuntimeGeneratedLineComponent;
class Actor;
} // namespace EngineCore

namespace Game {
class MissileExplosionVisitorBase;

class ElectroRayChainActor : public MissileActor, public ITweenStateChangeNotifyable {
    std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent> mLineComponent;

    glm::vec3 mElectroLineBegin;
    glm::vec3 mElectroLineEnd;

    std::weak_ptr<::EngineCore::Actor> mStartLineSpaceship;
    std::weak_ptr<::EngineCore::Actor> mEndLineSpaceship;

    float mChainingAnimationTimeDuration;
    float mChainingAnimationTime{0.0f};
    bool mIsPendingDisable{false};
    bool mIsChainingAnimationPlaying{true};

    std::shared_ptr<Tweener> mFadeoutTweener;

    std::shared_ptr<EngineObjectProperty<float>> mOpacity;

public:
    ElectroRayChainActor(const std::string& gameObjectName, const std::shared_ptr<EngineCore::SceneComponent>& rootComponent);

    void AttachTweener(std::shared_ptr<Tweener> tweener) override;

    void Tick(const float deltaTime) override;

    bool IsInsideLevel(const BoundingBox3D& boundingBox) const override;

    void TriggerSpawn(const glm::vec3& position, const eDamageDealerType ownerType) override;

    void TriggerExplosion() override;

    void TriggerExplosionFinished() override;

    void TriggerDisabled() override;

    std::shared_ptr<MissileExplosionVisitorBase> CreateMissileExplosionVisitor() override;

    void SetStartLineSpaceship(const std::weak_ptr<Actor>& startLineSpaceship);

    void SetEndLineSpaceship(const std::weak_ptr<Actor>& endLineSpaceship);

    void SetLineComponent(const std::shared_ptr<::EngineCore::RuntimeGeneratedLineComponent>& lineComponent);

    void SetIsPendingDisable(const bool value);

    bool IsPendingDisable() const;

private:
    void OnTweenStateChanged(const std::string& stateName) override;

    void DropState();

    glm::vec3 GetStartLinePosition();

    glm::vec3 GetEndLinePosition();

    void InitTweenerSubscriptions();

    void TriggerLifecycle_OnFadeOutStarted();

    void TriggerLifecycle_OnFadeOutFinished();
};
} // namespace Game