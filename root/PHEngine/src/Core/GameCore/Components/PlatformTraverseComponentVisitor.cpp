#include "PlatformTraverseComponentVisitor.h"

#include "Core/GameCore/Components/PhysicsComponents/PhysicsComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EnginePhysics;

namespace EngineCore {

PlatformTraverseComponentVisitorBase::PlatformTraverseComponentVisitorBase(std::weak_ptr<SceneComponent> ownerRootComponent)
    : mOwnerRootComp(ownerRootComponent)
{
}

void PlatformTraverseComponentVisitorBase::Init()
{
    if (auto rootCompSP = mOwnerRootComp.lock()) {
        mWorldTranslation = rootCompSP->GetTranslation();
        mStartTranslation = mWorldTranslation;
        mWorldTranslationDelta = glm::vec3(0);

        mWorldRotator = rootCompSP->GetRotator();
        mStartRotator = mWorldRotator;
    }
}

void PlatformTraverseComponentVisitorBase::LerpTransformation(const float time, const float transitionTime)
{
    const float currentTransitionTime = glm::clamp(time, 0.0f, transitionTime);

    const auto prevPosition = mWorldTranslation;
    mWorldTranslation = EngineMath::LerpVec3(currentTransitionTime, 0.0f, transitionTime, mStartTranslation, mEndTranslation);
    mWorldTranslationDelta = mWorldTranslation - prevPosition;

    mWorldRotator = EngineMath::SLerpQuat(currentTransitionTime / transitionTime, mStartRotator, mEndRotator);
}

glm::vec3 PlatformTraverseComponentVisitorBase::GetWorldTranslation() const
{
    return mWorldTranslation;
}

glm::vec3 PlatformTraverseComponentVisitorBase::GetWorldTranslationDelta() const
{
    return mWorldTranslationDelta;
}

glm::vec3 PlatformTraverseComponentVisitorBase::GetStartWorldTranslation() const
{
    return mStartTranslation;
}

glm::quat PlatformTraverseComponentVisitorBase::GetWorldRotator() const
{
    return mWorldRotator;
}

glm::quat PlatformTraverseComponentVisitorBase::GetWorldRotatorDelta() const
{
    return glm::quat();
}

glm::quat PlatformTraverseComponentVisitorBase::GetStartWorldRotator() const
{
    return glm::quat();
}

PlatformTraverseComponentVisitorNoPhys::PlatformTraverseComponentVisitorNoPhys(std::weak_ptr<SceneComponent> ownerRootComponent)
    : PlatformTraverseComponentVisitorBase(ownerRootComponent)
{
}

void PlatformTraverseComponentVisitorBase::CommitMovementStarted(const EulerAnglesTransform& targetTransform)
{
    mEndTranslation = mStartTranslation + targetTransform.Translation;
    mEndRotator = mStartRotator * EngineMath::EulerAnglesToQuat(targetTransform.RotationEulerAngles);
}

void PlatformTraverseComponentVisitorBase::CommitMovementFinished()
{
    mStartTranslation = mWorldTranslation;
    mStartRotator = mWorldRotator;
}

void PlatformTraverseComponentVisitorNoPhys::CommitMove()
{
    if (auto rootCompSP = mOwnerRootComp.lock()) {
        rootCompSP->SetTranslation(mWorldTranslation);
        rootCompSP->SetRotator(mWorldRotator);
    }
}

PlatformTraverseComponentVisitorWithPhys::PlatformTraverseComponentVisitorWithPhys(
    std::weak_ptr<SceneComponent> ownerRootComponent, std::weak_ptr<PhysicsComponent> ownerPhysComponent)
    : PlatformTraverseComponentVisitorBase(ownerRootComponent)
    , mOwnerPhysComp(ownerPhysComponent)
{
}

void PlatformTraverseComponentVisitorWithPhys::CommitMove()
{
    if (auto rootCompSP = mOwnerRootComp.lock()) {
        rootCompSP->SetTranslation(mWorldTranslation);
        rootCompSP->SetRotator(mWorldRotator);

        if (auto physCompSP = mOwnerPhysComp.lock()) {
            physCompSP->SetWorldTranslation(mWorldTranslation);
            physCompSP->SetWorldRotator(mWorldRotator);
        }
    }
}
} // namespace EngineCore