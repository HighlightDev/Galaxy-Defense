#include "TextureAtlasSpaceRequest.h"

#include "TextureAtlasFactory.h"

namespace Graphics {
size_t TextureAtlasSpaceRequest::m_requestId = 0;

TextureAtlasSpaceRequest::TextureAtlasSpaceRequest()
{
    TextureAtlasSpaceRequest::m_requestId += 1;
    MyRequestId = m_requestId;
}

} // namespace Graphics