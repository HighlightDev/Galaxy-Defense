#include "TEvent.h"

#include "Core/GameCore/ICamera.h"
#include "Core/GameCore/Components/Transform.h"

#include <utility>
#include <string>

namespace Event
{
   template class TEvent<AtomicEventPolicy<std::weak_ptr<Game::Transform>>>;
   template class TEvent<AtomicEventPolicy<std::string>>;
   template class TEvent<AtomicEventPolicy<Game::ICamera*>>;

   template class TEvent<MultipleEventPolicy<size_t>>;

   template <typename PolicyT>
   typename TEvent<PolicyT>::Event_t* TEvent<PolicyT>::m_instance = nullptr;
}