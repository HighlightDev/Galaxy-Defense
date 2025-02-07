#pragma once
#include "Component.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseBindings.h"

namespace EngineCore {
struct ComponentData;

class InputComponent : public Component {
    std::shared_ptr<KeyboardBindings> m_keyboardBindings;

    std::shared_ptr<MouseBindings> m_mouseBindings;

public:
    InputComponent(const std::shared_ptr<ComponentData>& componentData);

    virtual ~InputComponent();

    eComponentType GetComponentType() const override;

    // Game thread tick
    void Tick(const float deltaTime) override;

    void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

    std::vector<eKeyActionType> GetReleasedKeyActions();

    std::vector<eKeyActionType> GetPressedKeyActions();

    std::shared_ptr<KeyboardBindings> GetKeyboardBindings() const;

    std::shared_ptr<MouseBindings> GetMouseBindings() const;

    void SetIsReceivingMouseEvents(const bool receiveMouseEvents);

    void SetIsReceivingKeyboardEvents(const bool receiveKeyboardEvents);
};

} // namespace EngineCore
