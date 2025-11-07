#include "PlatformTraverseComponent.h"

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/ComponentData/MovementComponentData.h"
#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Event/KinematicBodyMovedEvent.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineCore::Scripts;

namespace EngineCore {
PlatformTraverseComponent::PlatformTraverseComponent(const std::shared_ptr<PlatformTraverseComponentData>& data)
    : Component(data->EngineObjectName)
    , mDestinationPoint(std::nullopt)
    , mTransitionTime(0.0f)
    , mMovementPoints(std::move(data->mRoutePoints))
{
}

PlatformTraverseComponent::~PlatformTraverseComponent()
{
}

void PlatformTraverseComponent::PostLevelInit()
{
    Component::PostLevelInit();

    if (const auto& spOwner = GetOwner().lock()) {
        const auto& rootComponent = spOwner->GetRootComponent();
        assert(rootComponent);

        const auto& physComponent = spOwner->GetPhysicsComponent();

        if (physComponent) {
            mBehaviorVisitor = std::make_unique<PlatformTraverseComponentVisitorWithPhys>(rootComponent, physComponent);
        } else {
            mBehaviorVisitor = std::make_unique<PlatformTraverseComponentVisitorNoPhys>(rootComponent);
        }

        mBehaviorVisitor->Init();
    }
}

eComponentType PlatformTraverseComponent::GetComponentType() const
{
    return PLATFORM_MOVEMENT_COMPONENT;
}

const std::vector<std::tuple<std::string, EulerAnglesTransform, float>>& PlatformTraverseComponent::GetMovementPoints() const
{
    return mMovementPoints;
}

void PlatformTraverseComponent::SetDestinationPointByIndex(const int32_t index)
{
    LogInfo("PlatformTraverseComponent::SetDestinationPoint: index: ", index);
    mCurrentPointIndex = index;
    assert(mCurrentPointIndex < mMovementPoints.size());
    mDestinationPoint = mMovementPoints.at(mCurrentPointIndex);
    const EulerAnglesTransform& transform = std::get<1>(mDestinationPoint.value());
    mBehaviorVisitor->CommitMovementStarted(transform);
}

void PlatformTraverseComponent::Move(const float deltaTimeSec)
{
    mTransitionTime += deltaTimeSec;

    const float transitionTime = std::get<2>(mDestinationPoint.value());

    mBehaviorVisitor->LerpTransformation(mTransitionTime, transitionTime);

    // If component is at final time position
    if (mTransitionTime > transitionTime || EngineMath::FloatsNearEqual(mTransitionTime, transitionTime)) {
        mTransitionTime = 0.0f;
        mDestinationPoint = std::nullopt;
    }
    mTransitionTime = fmod(mTransitionTime, transitionTime);
}

void PlatformTraverseComponent::Tick(const float deltaTimeSec)
{
    Component::Tick(deltaTimeSec);

    if (mDestinationPoint) {
        Move(deltaTimeSec);

        mBehaviorVisitor->CommitMove();

        if (const auto& spOwner = GetOwner().lock()) {
            if (auto physCompSP = spOwner->GetPhysicsComponent()) {
                EulerAnglesTransform transform;
                transform.Translation = mBehaviorVisitor->GetWorldTranslationDelta();
                const auto physDescriptor = physCompSP->GetDescriptor();
                KinematicBodyMovedGameThreadEvent::GetInstance()->SendEvent(
                    Event::eExecutionOrder::POST_EXECUTION, physDescriptor, transform);
            }
        }
    } else {
        if (mMovementPoints.size()) {
            const int32_t newIndex
                = (-1 != mCurrentPointIndex && mCurrentPointIndex < mMovementPoints.size() - 1) ? mCurrentPointIndex + 1 : 0;

            mBehaviorVisitor->CommitMovementFinished();
            SetDestinationPointByIndex(newIndex);
        }
    }
}

} // namespace EngineCore
