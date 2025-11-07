#include "HumanoidPlayerController.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Event/PlayerMovedEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GameCore/ThirdPersonCamera.h"

namespace EngineCore {

HumanoidPlayerController::HumanoidPlayerController(
    const std::shared_ptr<ACamera>& playerCamera, const std::shared_ptr<Actor>& actor)
    : ActorController(actor)
    , m_camera(playerCamera)
    , m_inputComponent()
{
}

HumanoidPlayerController::~HumanoidPlayerController()
{
    PhysicsComponentUpdatedGameThreadEvent::GetInstance()->RemoveListener(
        PhysicsComponentUpdatedGameThreadEvent::GetInstanceId());
}

void HumanoidPlayerController::Initialize()
{
    ActorController::Initialize();

    PhysicsComponentUpdatedGameThreadEvent::GetInstance()->AddListener(
        std::dynamic_pointer_cast<HumanoidPlayerController>(shared_from_this()));

    const auto& actorSp = m_actorWp.lock();
    const auto& rootComponent = actorSp->GetBaseRootComponent();
    assert(rootComponent);

    m_inputComponent = actorSp->GetInputComponent();
    assert(m_inputComponent);

    PlayerMovedGameThreadEvent::GetInstance()->SendEvent(eExecutionOrder::POST_EXECUTION, rootComponent->GetTransformWeakPtr());
}

void HumanoidPlayerController::ProcessEvent(
    const PhysicsComponentUpdatedGameThreadEvent* sender, const PhysicsComponentUpdatedGameThreadEvent::EventData_t& data)
{
    const std::string& actorName = std::move(std::get<0>(data));

    if (const auto& actorSp = m_actorWp.lock()) {
        if (actorSp->GetName() == actorName) {
            if (auto rootComponent = actorSp->GetBaseRootComponent()) {
                PlayerMovedGameThreadEvent::GetInstance()->SendEvent(
                    eExecutionOrder::PRE_EXECUTION, rootComponent->GetTransformWeakPtr());
            }
        }
    }
}

void HumanoidPlayerController::Tick(const float deltaTimeSec)
{
    if (const auto& actorSp = m_actorWp.lock()) {
        if (!actorSp->IsEnabled())
            return;

        const auto& rootComponent = actorSp->GetBaseRootComponent();

        if (actorSp->GetInputComponent()) {
            const auto& inputComponent = actorSp->GetInputComponent();

            const auto& mouseBindings = inputComponent->GetMouseBindings();
            if (mouseBindings->IsMouseMoveEventDirty()) {
                const auto& mouseMoveQueue = mouseBindings->FlushMouseMoveEvent();
                m_camera->SetRotation(mouseMoveQueue.z, mouseMoveQueue.w);
            }

            const auto& keyboardBindings = inputComponent->GetKeyboardBindings();
            if (keyboardBindings->HasPressedKeys()) {
                if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_FORWARD)) {
                    if (const auto& moveCompSp = m_movementComponentWp.lock()) {
                        moveCompSp->Move(deltaTimeSec);
                    }
                } else if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_LEFT)) {
                } else if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_RIGHT)) {
                } else if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_MOVE_BACK)) {
                }

                if (KeyState::PRESSED == keyboardBindings->GetKeyStateByActionType(eKeyActionType::ACTION_JUMP)) {
                    if (const auto& moveCompSp = m_movementComponentWp.lock()) {
                        moveCompSp->Jump();
                    }
                }
            }

            const std::vector<eKeyActionType>& currentFrameReleasedKeys = inputComponent->GetReleasedKeyActions();
            const std::vector<eKeyActionType>& currentFramePressedKeys = inputComponent->GetPressedKeyActions();

            // Buttons which have been released
            if (currentFrameReleasedKeys.size()) {
                auto moveForwardIt = std::find(
                    currentFrameReleasedKeys.begin(), currentFrameReleasedKeys.end(), eKeyActionType::ACTION_MOVE_FORWARD);
                if (moveForwardIt != currentFrameReleasedKeys.end()) {
                    actorSp->ChangeTweenerState("CharacterAnimation", "Idle");
                }
            }

            // Buttons which have been pressed
            if (currentFramePressedKeys.size()) {
                auto moveForwardIt = std::find(
                    currentFramePressedKeys.begin(), currentFramePressedKeys.end(), eKeyActionType::ACTION_MOVE_FORWARD);
                if (moveForwardIt != currentFramePressedKeys.end()) {
                    actorSp->ChangeTweenerState("CharacterAnimation", "Walking");
                }
            }
        }
    }
}

} // namespace EngineCore