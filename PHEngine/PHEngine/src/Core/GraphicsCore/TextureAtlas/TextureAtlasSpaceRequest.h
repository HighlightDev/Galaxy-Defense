#pragma once

#include "TextureAtlasHandler.h"

namespace Graphics
{
   class TextureAtlasSpaceRequest
   {
      static size_t m_requestId;

   public:

      size_t MyRequestId;

      TextureAtlasSpaceRequest();
   };
}

