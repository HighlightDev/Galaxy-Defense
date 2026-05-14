#include "Tweener.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Actor.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GameCore/Tweener/AnimationTweenController.h"
#include "Core/GameCore/Tweener/BooleanTweenController.h"
#include "Core/GameCore/Tweener/EulerAnglesRotationTweenController.h"
#include "Core/GameCore/Tweener/FloatTweenController.h"
#include "Core/GameCore/Tweener/Vec3QuadraticBezierTweenController.h"
#include "Core/GameCore/Tweener/Vec3TweenController.h"

#include <algorithm>

namespace EngineCore {
int32_t Tweener::s_id = 0;

Tweener::Tweener(
    const std::string& relPathFSM,
    const std::string& tweenerInnerName,
    std::shared_ptr<State> rootNode,
    std::vector<std::shared_ptr<State>>&& allStates)
    : m_id(s_id++)
    , mMyAllStates(std::move(allStates))
    , mRelPathTweener(relPathFSM)
    , mTweenerName(tweenerInnerName)
    , mStateNodeInitRoot(rootNode)
    , mCurrentStateNode(mStateNodeInitRoot)
    , bIsStateChangedDirty(false)
    , mChangedStateName("")
{
}

int32_t Tweener::GetId() const
{
    return m_id;
}

std::shared_ptr<ITweenController> GetPropertyTweenerController(const eEnginePropertyBindingType propertyType)
{
    std::shared_ptr<ITweenController> propertyController;

    if (eEnginePropertyBindingType::Animation == propertyType) {
        propertyController = std::make_shared<AnimationTweenController>();
    } else if (eEnginePropertyBindingType::FloatScalar == propertyType) {
        propertyController = std::make_shared<FloatTweenController>();
    } else if (eEnginePropertyBindingType::EulerAnglesRotation == propertyType) {
        propertyController = std::make_shared<EulerAnglesRotationTweenController>();
    } else if (eEnginePropertyBindingType::Boolean == propertyType) {
        propertyController = std::make_shared<BooleanTweenController>();
    } else if (eEnginePropertyBindingType::Vec3 == propertyType) {
        propertyController = std::make_shared<Vec3TweenController>();
    } else if (eEnginePropertyBindingType::Vec3QuadraticBezier == propertyType) {
        propertyController = std::make_shared<Vec3QuadraticBezierTweenController>();
    } else {
        ext_assert(false, "GetPropertyTweenerController: Unsupported property type");
    }

    return propertyController;
}

void Tweener::InitRootState()
{
    const std::string& rootStateName = mStateNodeInitRoot->GetStateName();

    std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> dstProperties
        = mStateNodeInitRoot->GetStateProperties();

    for (auto& dstNameAndPropertyPair : dstProperties) {
        std::shared_ptr<BaseStateProperty> dstProperty = dstNameAndPropertyPair.second;

        const auto propertyController = GetPropertyTweenerController(dstProperty->GetStatePropertyType());
        propertyController->InitWithPropsInstant(dstProperty);
    }

    SetTransitionValuesFinished(mStateNodeInitRoot);
}

void Tweener::DoTranstionInstantly(const std::string& dstStateName)
{
    // bTransitionEnabled and mCurrentActiveStateTransition must always be set/cleared together.
    // If this assert fires it means SetTransitionValuesFinished or DoTransition broke that invariant.
    ext_assert(
        mCurrentActiveStateTransition.has_value(),
        "DoTranstionInstantly: bTransitionEnabled is true but mCurrentActiveStateTransition is empty");

    if (mCurrentActiveStateTransition.has_value()) {
        for (std::shared_ptr<ITweenController>& controllerSp : CurrentActiveTransitionControllers) {
            controllerSp->OnTransitionFinished();
        }

        CurrentActiveTransitionControllers.clear();

        auto dstStateIt = std::find_if(
            mMyAllStates.begin(), mMyAllStates.end(), [=](const auto& state) { return state->GetStateName() == dstStateName; });
        ext_assert(dstStateIt != mMyAllStates.end(), "DoTranstionInstantly: Destination state not found: " + dstStateName);
        auto dstStateSp = (*dstStateIt);

        std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>> dstProperties
            = dstStateSp->GetStateProperties();
        for (auto& dstNameAndPropertyPair : dstProperties) {
            const std::shared_ptr<BaseStateProperty>& dstProperty = dstNameAndPropertyPair.second;

            const auto propertyController = GetPropertyTweenerController(dstProperty->GetStatePropertyType());
            propertyController->InitWithPropsInstant(dstProperty);
        }

        SetTransitionValuesFinished(dstStateSp);
    }
}

void Tweener::DoTransition(const std::string& dstStateName)
{
    const std::map<std::string /*dstStateName*/, StateTransition>& transitions = mCurrentStateNode->GetTransitions();

    ext_assert(transitions.count(dstStateName), "dstStateName: " + dstStateName);

    const StateTransition& transition = transitions.at(dstStateName);

    auto spDestination = transition.StateDestination.lock();
    auto spFrom = transition.StateFrom.lock();

    if (spDestination && spFrom) {
        ext_assert(
            spFrom->GetStateName() == mCurrentStateNode->GetStateName(),
            "DoTransition: Source state name does not match current state name");

        mCurrentActiveStateTransition = transition;
        mTransitionTime = 0.0f;
        mTransitionParameter = 0.0f;
        mTransitionDuration = transition.TransitionDuration;
        bTransitionEnabled = true;

        const std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>>& srcProperties
            = spFrom->GetStateProperties();
        const std::map<std::string /*Property Name*/, std::shared_ptr<BaseStateProperty>>& dstProperties
            = spDestination->GetStateProperties();

        for (auto& dstNameAndPropertyPair : dstProperties) {
            const std::string& name = dstNameAndPropertyPair.first;
            ext_assert(
                srcProperties.count(name),
                "DoTransition: missing transition from src to dst property, probably forgot to add property state to src state: "
                    + name);

            const std::shared_ptr<BaseStateProperty> srcProperty = srcProperties.at(name);
            const std::shared_ptr<BaseStateProperty> dstProperty = dstNameAndPropertyPair.second;

            const auto propertyController = GetPropertyTweenerController(dstProperty->GetStatePropertyType());
            CurrentActiveTransitionControllers.emplace_back(propertyController);
            propertyController->OnTransitionStarted(srcProperty, dstProperty, mTransitionDuration);
        }
    }
}

void Tweener::ChangeState(const std::string& dstStateName)
{
    if (bTransitionEnabled) {
        // Transition is not finished yet, but state should be already changed
        DoTranstionInstantly(dstStateName);
    } else {
        if (mCurrentStateNode->GetStateName() != dstStateName) {
            DoTransition(dstStateName);
        }
    }
}

void Tweener::SubscribeOnStateChange(const std::shared_ptr<ITweenStateChangeNotifyable>& observer)
{
    mStateChangedObservers.emplace_back(observer);
}

void Tweener::SetParentActor(const std::shared_ptr<Actor>& parent)
{
    mParentWp = parent;
}

std::weak_ptr<Actor> Tweener::GetParentActorWp() const
{
    return mParentWp;
}

void Tweener::SetTransitionValuesFinished(std::shared_ptr<State> newCurrentState)
{
    mTransitionParameter = 1.0f;
    mTransitionTime = 0.0f;
    mCurrentStateNode = newCurrentState;
    mCurrentActiveStateTransition = std::nullopt;
    bTransitionEnabled = false;
    mChangedStateName = newCurrentState->GetStateName();
    bIsStateChangedDirty = true;
}

void Tweener::NotifyStateChangedObservers()
{
    if (bIsStateChangedDirty) {
        bIsStateChangedDirty = false;

        // Snapshot the state name before invoking callbacks: a re-entrant ChangeState call
        // from within OnTweenStateChanged can overwrite mChangedStateName mid-loop,
        // causing subsequent observers to receive the wrong (new) state name.
        const std::string stateName = mChangedStateName;

        // Purge expired observers so the list does not grow unbounded.
        mStateChangedObservers.erase(
            std::remove_if(
                mStateChangedObservers.begin(),
                mStateChangedObservers.end(),
                [](const std::weak_ptr<ITweenStateChangeNotifyable>& wp) { return wp.expired(); }),
            mStateChangedObservers.end());

        for (const auto& observerWp : mStateChangedObservers) {
            if (const auto& observerSp = observerWp.lock()) {
                observerSp->OnTweenStateChanged(mChangedStateName);
            }
        }
    }
}

void Tweener::CleanUp()
{
    LogInfo("Tweener::CleanUp: name: ", mTweenerName);
    mMyAllStates.clear();
    mPropertyBindings.clear();
    CurrentActiveTransitionControllers.clear();
    mStateChangedObservers.clear();
}

std::shared_ptr<PropertyBinding> Tweener::GetPropertyBindingByName(const std::string& name) const
{
    ext_assert(mPropertyBindings.count(name), "GetPropertyBindingByName: Property binding not found: " + name);
    return mPropertyBindings.at(name);
}

void Tweener::AddPropertyBinding(const std::string& propBindingName, std::shared_ptr<PropertyBinding> binding)
{
    ext_assert(binding, "AddPropertyBinding: binding is null: " + propBindingName);
    mPropertyBindings[propBindingName] = binding;
}

void Tweener::Tick(const float deltaTimeSec)
{
    // process current transition
    if (bTransitionEnabled && mCurrentActiveStateTransition.has_value()) {
        if (auto spDestination = mCurrentActiveStateTransition->StateDestination.lock()) {
            std::shared_ptr<State> stateTo = spDestination;

            mTransitionTime += deltaTimeSec;

            if (mTransitionTime > mTransitionDuration) {
                SetTransitionValuesFinished(stateTo);
            } else {
                mTransitionParameter = mTransitionTime / mTransitionDuration;
            }

            for (const auto& controllerSp : CurrentActiveTransitionControllers) {
                if (bTransitionEnabled) {
                    controllerSp->OnTransitionUpdate(deltaTimeSec, mTransitionParameter);
                } else {
                    controllerSp->OnTransitionFinished();
                }
            }

            if (!bTransitionEnabled) {
                CurrentActiveTransitionControllers.clear();
            }
        }
    }
}

bool Tweener::IsTransitionActive() const
{
    return bTransitionEnabled;
}

float Tweener::GetTransitionParameter() const
{
    return mTransitionParameter;
}

std::shared_ptr<State> Tweener::GetCurrentState() const
{
    return mCurrentStateNode;
}

std::string Tweener::GetRelPathTweener() const
{
    return mRelPathTweener;
}

const std::string& Tweener::GetTweenerName() const
{
    return mTweenerName;
}
} // namespace EngineCore