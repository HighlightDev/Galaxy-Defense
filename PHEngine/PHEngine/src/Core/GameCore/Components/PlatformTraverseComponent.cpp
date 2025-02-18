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
    , mTime(0.0f)
    , mMovementPoints(data->mRoutePoints)
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

void PlatformTraverseComponent::SetDestinationPoint(const std::string& pointName)
{
    mLastDestinationPoint = pointName;
    const auto foundPointIt = std::find_if(mMovementPoints.cbegin(), mMovementPoints.cend(), [&pointName](const auto& pointData) {
        return std::get<0>(pointData) == pointName;
    });
    assert(foundPointIt != mMovementPoints.cend());
    mDestinationPoint = *(foundPointIt);
    const EulerAnglesTransform& transform = std::get<1>(mDestinationPoint.value());
    mBehaviorVisitor->CommitMovementStarted(transform);
}

void PlatformTraverseComponent::Move(const float deltaTime)
{
    mTime += deltaTime;

    const float transitionTime = std::get<2>(mDestinationPoint.value());

    mBehaviorVisitor->LerpTransformation(mTime, transitionTime);

    // If component is at final time position
    if (EngineMath::FloatsNearEqual(mTime, transitionTime)) {
        mTime = 0.0f;
        mDestinationPoint = std::nullopt;
    }
    mTime = fmod(mTime, transitionTime);
}

void PlatformTraverseComponent::Tick(const float deltaTime)
{
    Component::Tick(deltaTime);

    if (mDestinationPoint) {
        Move(deltaTime);

        mBehaviorVisitor->CommitMove();

        EulerAnglesTransform transform;
        transform.Translation = mBehaviorVisitor->GetWorldTranslationDelta();

        if (const auto& spOwner = GetOwner().lock()) {
            if (auto physCompSP = spOwner->GetPhysicsComponent()) {
                const auto physDescriptor = physCompSP->GetDescriptor();
                KinematicBodyMovedGameThreadEvent::GetInstance()->SendEvent(
                    Event::eExecutionOrder::POST_EXECUTION, physDescriptor, transform);
            }
        }
    } else {
        if (mMovementPoints.size()) {
            auto foundPointIt = std::find_if(
                mMovementPoints.begin(), mMovementPoints.end(), [pointName = mLastDestinationPoint](const auto& pointData) {
                    return std::get<0>(pointData) == pointName;
                });
            auto itNext = mMovementPoints.begin();
            if (foundPointIt != mMovementPoints.end()) {
                const int32_t pointIndex = std::distance(mMovementPoints.begin(), foundPointIt);
                if (pointIndex < mMovementPoints.size() - 1) {
                    itNext = std::next(foundPointIt, 1);
                }
            }

            mBehaviorVisitor->CommitMovementFinished();
            SetDestinationPoint(std::get<0>(*(itNext)));
        }
    }
}

void PlatformTraverseComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
{
    auto& actorData = GetSerializeDataActor(dataContainer);

    std::shared_ptr<SerializeDataPlatformTraverseComponent> data = std::make_shared<SerializeDataPlatformTraverseComponent>();
    data->ComponentName = EngineObjectName;

    actorData.ComponentsData.emplace_back(data);
}
} // namespace EngineCore
