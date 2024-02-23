#include "TEvent.h"

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseEventEnums.h"
#include "Core/GameCore/GUI/HudText/HudTextField.h"
#include "Core/GameCore/Event/TextEvent.h"
#include "Core/GameCore/Input/InputDeviceKeyData.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"

using namespace Graphics::Texture;

namespace Event
{
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::weak_ptr<Transform>>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::string>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<ACamera*>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::vector<KeyboardKeysData>>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<glm::ivec4>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eMouseScrollDirection>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eMouseEventTargetReceiverType, std::vector<MouseKeysData>>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<glm::ivec2, std::vector<MouseKeysData>>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::weak_ptr<EnginePhysics::PhysicsDescriptor>, EulerAnglesTransform>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<bool>>;
   template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<::Graphics::ViewPortInfo>>;

   template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<eTextureType>>;
   template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<size_t>>;
   template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string>>;
   template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>;
   template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::shared_ptr<HudTextField>, eRegisterType, bool>>;
   template class TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::weak_ptr<HudTextField>, eTextChangedDataType>>;

   template class TEvent<eEventThreadType::GAME_THREAD, NoDataEventPolicy>;

   template class TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<std::vector<KeyboardKeysData>>>;
   template class TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<eMouseScrollDirection>>;
   template class TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<std::vector<MouseKeysData>>>;
   template class TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<glm::ivec4>>;
   template class TEvent<eEventThreadType::LUA_THREAD, SingleDataEventPolicy<::Graphics::ViewPortInfo>>;

   template class TEvent<eEventThreadType::LUA_THREAD, MultipleDataEventPolicy<std::string /*event header*/, std::string /*json parameters*/>>;

}