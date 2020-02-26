#include "LazyTextureAtlasObtainer.h"
#include "TextureAtlasFactory.h"

namespace Graphics
{
   size_t LazyTextureAtlasObtainer::m_requestId = 0;

   LazyTextureAtlasObtainer::LazyTextureAtlasObtainer()
   {
      LazyTextureAtlasObtainer::m_requestId += 1;
      MyRequestId = m_requestId;
   }

   std::shared_ptr<TextureAtlasHandler> LazyTextureAtlasObtainer::GetTextureAtlasCellResource() const
   {
      return TextureAtlasFactory::GetInstance()->GetTextureAtlasCellByRequestId(MyRequestId);
   }

}