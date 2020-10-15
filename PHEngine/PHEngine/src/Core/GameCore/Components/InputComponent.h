#pragma once
#include "Component.h"
#include "Core/GameCore/Input/KeyboardBindings.h"

namespace Game
{

   class InputComponent :
      public Component
   {

      KeyboardBindings m_keyboardBindings;

   public:

      InputComponent(const std::string& gameObjectName);

      virtual ~InputComponent();

      virtual ComponentType GetComponentType() const override;

      // Game thread tick
      virtual void Tick(const float deltaTime) override;

      const KeyboardBindings& GetKeyboardBindings() const;
   };

}
