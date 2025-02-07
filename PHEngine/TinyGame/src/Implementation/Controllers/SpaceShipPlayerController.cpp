#include "SpaceShipPlayerController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Implementation/Events/MainPlayerActionEvent.h"

#include <algorithm>
#include <iostream>

namespace Game {

SpaceShipPlayerController::SpaceShipPlayerController(
    const std::shared_ptr<ACamera>& playerCamera, const std::shared_ptr<Actor>& actor)
    : ActorController(actor)
    , m_camera()
    , mCurrentState("")
    , mLevelBounds()
    , mSpaceShipPrimitiveComponent()
{
    assert((eCameraType::MAIN_FIRST_PERSON_CAMERA & playerCamera->GetCameraType()) == eCameraType::MAIN_FIRST_PERSON_CAMERA);
    m_camera = std::static_pointer_cast<FirstPersonCamera>(playerCamera);
}

SpaceShipPlayerController::~SpaceShipPlayerController()
{
}

void SpaceShipPlayerController::Initialize()
{
    ActorController::Initialize();

    const auto& actorSp = m_actorWp.lock();
    assert(actorSp);
    const auto& rootComponent = actorSp->GetBaseRootComponent();
    assert(rootComponent);
    PlayerMovedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());

    const auto& primitiveComponents = actorSp->GetComponentsByType<PrimitiveComponent>();
    assert(primitiveComponents.size());

    const auto maxBoundPrimitiveComponentIt = std::max_element(
        primitiveComponents.begin(),
        primitiveComponents.end(),
        [](const auto& leftPrimitiveComp, const auto& rightPrimitiveComp) {
            const auto& leftHalfExtent = leftPrimitiveComp->GetTransformedBoundingBox().GetHalfExtent();
            const auto& rightHalfExtent = rightPrimitiveComp->GetTransformedBoundingBox().GetHalfExtent();
            return glm::length2(leftHalfExtent) < glm::length2(rightHalfExtent);
        });

    assert(primitiveComponents.end() != maxBoundPrimitiveComponentIt);
    mSpaceShipPrimitiveComponent = *maxBoundPrimitiveComponentIt;
}

void SpaceShipPlayerController::SetLevelBounds(const BoundingBox3D& levelBounds)
{
    mLevelBounds = levelBounds;
}

std::shared_ptr<FirstPersonCamera> SpaceShipPlayerController::GetCamera() const
{
    return m_camera;
}

void SpaceShipPlayerController::Tick(float deltaTime)
{
    const auto& actorSp = m_actorWp.lock();
    assert(actorSp);

    if (!actorSp->IsEnabled())
        return;

    auto spaceshipTweener = actorSp->GetTweenerByName("SpaceshipMovement");
    assert(spaceshipTweener);

    if ("" == mCurrentState) {
        mCurrentState = spaceshipTweener->GetCurrentState()->GetStateName();
    }

    if (actorSp->GetInputComponent()) {
        bool bMoveCommitted = true;
        const auto& inputComponent = actorSp->GetInputComponent();

        const auto& mouseBindings = inputComponent->GetMouseBindings();

        if (mouseBindings->IsMouseScrollEventDirty()) {
            const auto scrollDirection = mouseBindings->FlushMouseScrollEvent();
            MainPlayerActionEvent::GetInstance()->SendEvent(
                eExecutionOrder::POST_EXECUTION,
                eMouseScrollDirection::ZoomIn == scrollDirection ? eMainPlayerActionEnum::SELECT_NEXT_MISSILE_TYPE
                                                                 : eMainPlayerActionEnum::SELECT_PREV_MISSILE_TYPE);
        }

        if (KeyState::PRESSED == mouseBindings->GetKeyState(EngineCore::eMouseKeys::MouseButtonLeft)) {
            MainPlayerActionEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, eMainPlayerActionEnum::SHOOT);
        }

        const auto& keyboardBindings = inputComponent->GetKeyboardBindings();
        if (keyboardBindings->HasPressedKeys()) {
            const auto& levelAreaMinPos = mLevelBounds.GetMin();
            const auto& levelAreaMaxPos = mLevelBounds.GetMax();

            const auto& transformedBoundBox = mSpaceShipPrimitiveComponent->GetTransformedBoundingBox();
            const auto& shipMaxPos = transformedBoundBox.GetMax();
            const auto& shipMinPos = transformedBoundBox.GetMin();

            glm::vec3 direction(0.0f);

            if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_FORWARD)) {
                if (shipMaxPos.z <= levelAreaMaxPos.z) {
                    direction.z = 1.0f;
                }

                if ("s_fly_forward" != mCurrentState) {
                    mCurrentState = "s_fly_forward";
                    actorSp->ChangeTweenerState("SpaceshipMovement", "s_fly_forward");
                }
            } else if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_LEFT)) {
                if (shipMaxPos.x <= levelAreaMaxPos.x) {
                    direction.x = 1.0f;
                }

                if ("s_fly_left" != mCurrentState) {
                    mCurrentState = "s_fly_left";
                    actorSp->ChangeTweenerState("SpaceshipMovement", "s_fly_left");
                }
            } else if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_RIGHT)) {
                if (shipMinPos.x >= levelAreaMinPos.x) {
                    direction.x = -1.0f;
                }

                if ("s_fly_right" != mCurrentState) {
                    mCurrentState = "s_fly_right";
                    actorSp->ChangeTweenerState("SpaceshipMovement", "s_fly_right");
                }
            } else if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_BACK)) {
                if (shipMinPos.z >= (levelAreaMinPos.z + 25.0f)) {
                    direction.z = -1.0f;
                }

                if ("s_fly_back" != mCurrentState) {
                    mCurrentState = "s_fly_back";
                    actorSp->ChangeTweenerState("SpaceshipMovement", "s_fly_back");
                }
            } else {
                bMoveCommitted = false;
            }

            if (bMoveCommitted) {
                if (const auto& movementComponentSp = m_movementComponentWp.lock()) {
                    movementComponentSp->SetDirection(direction);
                    movementComponentSp->Move(deltaTime);
                }
            }
        } else {
            bMoveCommitted = false;
        }

        if (!bMoveCommitted) {
            if ("s_idle" != mCurrentState) {
                mCurrentState = "s_idle";
                actorSp->ChangeTweenerState("SpaceshipMovement", "s_idle");
            }
        }

        if (bMoveCommitted || spaceshipTweener->IsTransitionActive()) {
            const auto& rootComponent = actorSp->GetBaseRootComponent();
            PlayerMovedEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
        }
    }
}
} // namespace Game