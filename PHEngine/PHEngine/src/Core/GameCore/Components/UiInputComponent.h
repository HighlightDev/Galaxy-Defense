#pragma once
#include "Component.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/UiMouseBindings.h"

namespace EngineCore {
struct ComponentData;

class UiInputComponent : public Component {
    std::shared_ptr<KeyboardBindings> m_keyboardBindings;

    std::shared_ptr<UiMouseBindings> m_uiMouseBindings;

public:
    UiInputComponent(const std::shared_ptr<ComponentData>& componentData);

    virtual ~UiInputComponent() = default;

    eComponentType GetComponentType() const override;

    // Game thread tick
    void Tick(const float deltaTimeSec, const float playSpeed) override;

    std::vector<eKeyActionType> GetReleasedKeyActions();

    std::vector<eKeyActionType> GetPressedKeyActions();

    std::shared_ptr<KeyboardBindings> GetKeyboardBindings() const;

    std::shared_ptr<UiMouseBindings> GetMouseBindings() const;

    void SetIsReceivingMouseEvents(const bool receiveMouseEvents);

    void SetIsReceivingKeyboardEvents(const bool receiveKeyboardEvents);
};

} // namespace EngineCore
