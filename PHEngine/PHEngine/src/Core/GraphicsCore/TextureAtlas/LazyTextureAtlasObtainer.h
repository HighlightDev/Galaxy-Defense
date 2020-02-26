#pragma once

#include "TextureAtlasHandler.h"

namespace Graphics
{
   class LazyTextureAtlasObtainer
   {
      static size_t m_requestId;

   public:

      size_t MyRequestId;

      LazyTextureAtlasObtainer();

      std::shared_ptr<TextureAtlasHandler> GetTextureAtlasCellResource() const;
   };
}

