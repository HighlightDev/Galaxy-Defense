#pragma once

#include "DeferredResource.h"

namespace Resources {

   struct IDeferredResourceCreator 
   {
      virtual std::shared_ptr<IDeferredResourceBase> GetDeferredResource() = 0;
   };
}