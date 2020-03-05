#pragma once

#include <memory>

#include "Core/GraphicsCore/Mesh/Skin.h"

using namespace Graphics::Mesh;

namespace Game {

   class Batch
   {

      std::shared_ptr<Skin> mMutualSkin;
      
      size_t mCountOfInstances;



   };

}
