#pragma once
#include "Component.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseBindings.h"

namespace Game
{
   struct InputComponentData;

   class InputComponent :
      public Component
   {

      KeyboardBindings m_keyboardBindings;

      MouseBindings m_mouseBindings;

   public:

      InputComponent(const InputComponentData& componentData);

      virtual ~InputComponent();

      virtual ComponentType GetComponentType() const override;

      // Game thread tick
      virtual void Tick(const float deltaTime) override;

      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      std::vector<eKeyActionType> GetReleasedKeyActions();

      std::vector<eKeyActionType> GetPressedKeyActions();

      KeyboardBindings& GetKeyboardBindings();

      MouseBindings& GetMouseBindings();
   };

}
