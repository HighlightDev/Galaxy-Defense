#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

using namespace EnginePhysics;

namespace Game
{
   struct PhysicsComponentData : public ComponentData
   {
      PhysicsComponentData(PhysicsDescriptor* descriptor)
         : ComponentData()
         , mPhysicsDescriptor(descriptor)
      {
      }

      PhysicsDescriptor* mPhysicsDescriptor;
   };

}
