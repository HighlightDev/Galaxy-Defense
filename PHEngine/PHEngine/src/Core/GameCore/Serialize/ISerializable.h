#pragma once

#include <cereal/archives/binary.hpp>

#include "SerializeData/SerializeDataContainer.h"

namespace Game
{
   struct ISerializable
   {
      virtual void CollectDataForSerialization(SerializeDataContainer& dataContainer) = 0;
   };
}