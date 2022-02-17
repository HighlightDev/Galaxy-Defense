#include "TEvent.h"

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/Event/KeyboardInputEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseEventEnums.h"

using namespace Graphics::Texture;

namespace Event
{
   template class TEvent<SingleDataEventPolicy<std::weak_ptr<Game::Transform>>>;
   template class TEvent<SingleDataEventPolicy<std::string>>;
   template class TEvent<SingleDataEventPolicy<Game::ACamera*>>;
   template class TEvent<SingleDataEventPolicy<std::vector<KeyboardData>>>;
   template class TEvent<SingleDataEventPolicy<glm::ivec4>>;
   template class TEvent<SingleDataEventPolicy<Game::eMouseScrollDirection>>;
   template class TEvent<SingleDataEventPolicy<EnginePhysics::PhysicsDescriptor*, Game::EulerAnglesTransform>>;

   template class TEvent<MultipleDataEventPolicy<TextureType>>;
   template class TEvent<MultipleDataEventPolicy<size_t>>;

}