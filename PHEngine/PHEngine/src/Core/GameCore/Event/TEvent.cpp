#include "TEvent.h"

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/Event/KeyboradInputEvent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"
#include "Core/GraphicsCore/Texture/ITexture.h"

#include <utility>
#include <string>

using namespace Graphics::Texture;

namespace Event
{
   template class TEvent<AtomicEventPolicy<std::weak_ptr<Game::Transform>>>;
   template class TEvent<AtomicEventPolicy<std::string>>;
   template class TEvent<AtomicEventPolicy<Game::ACamera*>>;
   template class TEvent<AtomicEventPolicy<KeyboardEventData>>;
   template class TEvent<AtomicEventPolicy<EnginePhysics::PhysicsDescriptor*, Game::Transform>>;
   template class TEvent<MultipleEventPolicy<TextureType>>;

   template class TEvent<MultipleEventPolicy<size_t>>;

   template <typename PolicyT>
   typename TEvent<PolicyT>::Event_t* TEvent<PolicyT>::m_instance = nullptr;
}