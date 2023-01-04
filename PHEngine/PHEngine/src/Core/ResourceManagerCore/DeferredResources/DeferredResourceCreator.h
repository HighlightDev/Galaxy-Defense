#pragma once

#include "DeferredResource.h"

namespace Resources {

   struct IDeferredResourceCreator 
   {
      virtual std::shared_ptr<IDeferredResource> GetDeferredResource() = 0;
   };
}