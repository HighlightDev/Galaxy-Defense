#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

using namespace EnginePhysics;

namespace EngineCore
{
   struct PhysicsComponentData : public ComponentData
   {
      PhysicsComponentData(const std::string& gameObjectName, PhysicsDescriptor* descriptor)
         : ComponentData(gameObjectName)
         , mPhysicsDescriptor(descriptor)
      {
      }

      PhysicsDescriptor* mPhysicsDescriptor;
   };

}
