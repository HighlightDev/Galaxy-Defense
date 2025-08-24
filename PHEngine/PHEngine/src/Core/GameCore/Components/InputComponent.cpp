#include "InputComponent.h"

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Input/InputManager.h"

namespace EngineCore {

InputComponent::InputComponent(const std::shared_ptr<ComponentData>& componentData)
    : Component(componentData->EngineObjectName)
    , m_keyboardBindings(std::make_shared<KeyboardBindings>(std::make_shared<DefaultKeyboardBindings>()))
    , m_mouseBindings(std::make_shared<MouseBindings>())
{
    m_keyboardBindings->Initialize();
    m_mouseBindings->Initialize();
}

InputComponent::~InputComponent()
{
}

eComponentType InputComponent::GetComponentType() const
{
    return INPUT_COMPONENT;
}

void InputComponent::SetIsReceivingMouseEvents(const bool receiveMouseEvents)
{
    m_mouseBindings->SetIsReceivingMouseEvents(receiveMouseEvents);
}

void InputComponent::SetIsReceivingKeyboardEvents(const bool receiveKeyboardEvents)
{
    m_keyboardBindings->SetIsReceivingKeyboardEvents(receiveKeyboardEvents);
}

std::vector<eKeyActionType> InputComponent::GetReleasedKeyActions()
{
    std::vector<eKeyActionType> result;
    std::shared_ptr<IActionBinding> actionBindings = m_keyboardBindings->GetActionBindings();
    const std::vector<eKeyboardKeys>& releasedKeys = m_keyboardBindings->GetReleasedKeys();
    std::for_each(releasedKeys.begin(), releasedKeys.end(), [&](const auto& key) {
        result.push_back(actionBindings->GetMappedWithKeyAction(key));
    });
    return result;
}

std::vector<eKeyActionType> InputComponent::GetPressedKeyActions()
{
    std::vector<eKeyActionType> result;
    std::shared_ptr<IActionBinding> actionBindings = m_keyboardBindings->GetActionBindings();
    const std::vector<eKeyboardKeys>& pressedKeys = m_keyboardBindings->GetPressedKeys();
    std::for_each(pressedKeys.begin(), pressedKeys.end(), [&](const auto& key) {
        result.push_back(actionBindings->GetMappedWithKeyAction(key));
    });
    return result;
}

std::shared_ptr<KeyboardBindings> InputComponent::GetKeyboardBindings() const
{
    return m_keyboardBindings;
}

std::shared_ptr<MouseBindings> InputComponent::GetMouseBindings() const
{
    return m_mouseBindings;
}

void InputComponent::Tick(const float deltaTime)
{
}
} // namespace EngineCore