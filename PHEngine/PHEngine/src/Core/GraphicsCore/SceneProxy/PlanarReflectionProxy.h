#pragma once

#include "SceneProxyBase.h"

namespace Game {
   class PlanarReflectionComponent;
}

namespace Graphics
{
   using Game::PlanarReflectionComponent;

   class PlanarReflectionProxy
      : public SceneProxyBase
   {
   public:
      PlanarReflectionProxy(PlanarReflectionComponent const* planarReflectionComponent);

      ~PlanarReflectionProxy();
   };
}

