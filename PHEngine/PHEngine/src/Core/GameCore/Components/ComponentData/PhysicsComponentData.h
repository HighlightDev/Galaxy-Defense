#pragma once

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

#include <memory>

using namespace EnginePhysics;

namespace EngineCore
{
   struct PhysicsComponentData : public ComponentData
   {
      PhysicsComponentData(const std::string &gameObjectName,
                           const std::shared_ptr<PhysicsDescriptor> &descriptor)
          : ComponentData(gameObjectName),
            mPhysicsDescriptor(descriptor)
      {
      }

      std::shared_ptr<PhysicsDescriptor> mPhysicsDescriptor;
   };

}
