#include "DeferredResource.h"

#include "Core/GraphicsCore/Texture/ITexture.h"

using namespace Graphics;
using namespace Graphics::Texture;

namespace Resources
{
   template struct DeferredResource<std::shared_ptr<ITexture>, eDeferredResourceType::TEXTURE>;
}