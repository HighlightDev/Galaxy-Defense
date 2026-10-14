#include "DeferredResourceController.h"

#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics::Texture;

namespace Resources {

template struct DeferredResourceController<std::shared_ptr<ITexture>, eDeferredResourceType::TEXTURE>;
}
