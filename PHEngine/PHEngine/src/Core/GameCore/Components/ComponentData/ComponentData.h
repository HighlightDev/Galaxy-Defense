#pragma once

#include "Core/GameCore/Components/ComponentType.h"

namespace Game
{

	struct ComponentData
	{

		ComponentData() { }

      virtual uint64_t GetType() const
      {
         return COMPONENT;
      }
	};

}