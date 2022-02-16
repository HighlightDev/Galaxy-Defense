#include "InputComponent.h"
#include "Core/GameCore/Input/InputManager.h"
#include "Core/GameCore/Components/ComponentData/InputComponentData.h"

namespace Game
{

   InputComponent::InputComponent(const InputComponentData& componentData)
      : Component(componentData.GameObjectName)
      , m_keyboardBindings(std::make_shared<DefaultKeyboardBindings>())
      , m_mouseBindings()
   {
   }

   InputComponent::~InputComponent()
   {
   }

   ComponentType InputComponent::GetComponentType() const
   {
      return INPUT_COMPONENT;
   }

   std::vector<eKeyActionType> InputComponent::GetReleasedKeyActions()
   {
      std::vector<eKeyActionType> result;
      std::shared_ptr<IActionBinding> actionBindings = m_keyboardBindings.GetActionBindings();
      const std::vector<Keys>& releasedKeys = m_keyboardBindings.GetReleasedKeysOnCurrentTickAndInvalidateVector();
      std::for_each(releasedKeys.begin(), releasedKeys.end(), [&](const auto& key) { result.push_back(actionBindings->GetMappedWithKeyAction(key)); });
      return result;
   }

   std::vector<eKeyActionType> InputComponent::GetPressedKeyActions()
   {
      std::vector<eKeyActionType> result;
      std::shared_ptr<IActionBinding> actionBindings = m_keyboardBindings.GetActionBindings();
      const std::vector<Keys>& pressedKeys = m_keyboardBindings.GetPressedKeysOnCurrentTickAndInvalidateVector();
      std::for_each(pressedKeys.begin(), pressedKeys.end(), [&](const auto& key) { result.push_back(actionBindings->GetMappedWithKeyAction(key)); });
      return result;
   }

   void InputComponent::CollectDataForSerialization(SerializeDataContainer& dataContainer)
   {
      auto& dataActor = GetSerializeDataActor(dataContainer);
      std::shared_ptr<SerializeDataInputComponent> inputComp = std::make_shared<SerializeDataInputComponent>();
      inputComp->ComponentName = GameObjectName;
      dataActor.ComponentsData.emplace_back(inputComp);
   }

   KeyboardBindings& InputComponent::GetKeyboardBindings() {
      return m_keyboardBindings;
   }
   
   MouseBindings& InputComponent::GetMouseBindings() {
      return m_mouseBindings;
   }

   void InputComponent::Tick(const float deltaTime) { }
}