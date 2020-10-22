#pragma once

#include "Core/GameCore/Serialize/SerializeData/SerializeData.h"

namespace Graphics {
   class IMaterial;
}

namespace Game {

   using Graphics::IMaterial;

   class SerializeHelper
   {
   public:
      
      static SerializeDataMaterial GetSerializeDataMaterial(std::shared_ptr<IMaterial> materialInstance);
   };

}

