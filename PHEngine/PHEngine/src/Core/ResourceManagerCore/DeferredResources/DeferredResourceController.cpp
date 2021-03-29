#include "DeferredResourceController.h"

namespace Resources {
   
   template struct DeferredResourceController<std::shared_ptr<ITexture>, eResourceType::TEXTURE>;
}
