#pragma once

#include <cereal/archives/binary.hpp>

namespace Game
{
   struct ISerializable
   {
      virtual void Serialize(cereal::BinaryOutputArchive& archive) = 0;
   };
}