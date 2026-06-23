#include "UiInputComponent.h"

#include "Core/GameCore/Components/ComponentData/ComponentData.h"

namespace EngineCore {

UiInputComponent::UiInputComponent(const std::shared_ptr<ComponentData>& componentData)
    : Component(componentData->EngineObjectName)
    , m_keyboardBindings(std::make_shared<KeyboardBindings>(std::make_shared<DefaultKeyboardBindings>()))
    , m_uiMouseBindings(std::make_shared<UiMouseBindings>())
{
    m_keyboardBindings->Initialize();
    m_uiMouseBindings->Initialize();
}

eComponentType UiInputComponent::GetComponentType() const
{
    return INPUT_COMPONENT;
}

void UiInputComponent::SetIsReceivingMouseEvents(const bool receiveMouseEvents)
{
    m_uiMouseBindings->SetIsReceivingMouseEvents(receiveMouseEvents);
}

void UiInputComponent::SetIsReceivingKeyboardEvents(const bool receiveKeyboardEvents)
{
    m_keyboardBindings->SetIsReceivingKeyboardEvents(receiveKeyboardEvents);
}

std::vector<eKeyActionType> UiInputComponent::GetReleasedKeyActions()
{
    std::vector<eKeyActionType> result;
    std::shared_ptr<IActionBinding> actionBindings = m_keyboardBindings->GetActionBindings();
    const std::vector<eKeyboardKeys>& releasedKeys = m_keyboardBindings->GetReleasedKeys();
    std::for_each(releasedKeys.begin(), releasedKeys.end(), [&](const auto& key) {
        result.push_back(actionBindings->GetMappedWithKeyAction(key));
    });
    return result;
}

std::vector<eKeyActionType> UiInputComponent::GetPressedKeyActions()
{
    std::vector<eKeyActionType> result;
    std::shared_ptr<IActionBinding> actionBindings = m_keyboardBindings->GetActionBindings();
    const std::vector<eKeyboardKeys>& pressedKeys = m_keyboardBindings->GetPressedKeys();
    std::for_each(pressedKeys.begin(), pressedKeys.end(), [&](const auto& key) {
        result.push_back(actionBindings->GetMappedWithKeyAction(key));
    });
    return result;
}

std::shared_ptr<KeyboardBindings> UiInputComponent::GetKeyboardBindings() const
{
    return m_keyboardBindings;
}

std::shared_ptr<UiMouseBindings> UiInputComponent::GetMouseBindings() const
{
    return m_uiMouseBindings;
}

void UiInputComponent::Tick(const float deltaTimeSec, const float playSpeed)
{
}
} // namespace EngineCore