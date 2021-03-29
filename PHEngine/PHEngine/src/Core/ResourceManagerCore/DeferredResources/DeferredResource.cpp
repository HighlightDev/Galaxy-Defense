#include "DeferredResource.h"

namespace Resources
{
   template struct IDeferredResource<std::shared_ptr<ITexture>, eResourceType::TEXTURE>;
}