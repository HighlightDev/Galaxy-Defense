#pragma once

#include <stdint.h>
#include <gl/glew.h>
#include <stdint.h>
#include <type_traits>

namespace Graphics
{
	namespace Texture
	{

		struct TexParams
		{

			int32_t TexBufferWidth;
			int32_t TexBufferHeight;
			int32_t TexTarget;
			int32_t TexMagFilter;
			int32_t TexMinFilter;
			int32_t TexMipLvl;
			int32_t TexPixelInternalFormat;
			int32_t TexPixelFormat;
			int32_t TexPixelType;
			int32_t TexWrapMode;

			TexParams(
				int32_t texBufferWidth,
				int32_t texBufferHeight,
				int32_t texTarget = GL_TEXTURE_2D,
				int32_t magFilter = GL_LINEAR,
				int32_t minFilter = GL_LINEAR,
				int32_t texMipLvl = 0,
				int32_t texPixelInternalFormat = GL_RGB,
				int32_t texPixelFormat = GL_RGB,
				int32_t texPixelType = GL_UNSIGNED_BYTE,
				int32_t texWrapMode = GL_REPEAT,
            bool bUnique_resource = false);

			TexParams();

			~TexParams();

      private:

         friend struct std::hash<TexParams>;

         static size_t unique_index;

         size_t UniqueIndex;

      public:

         bool operator==(const TexParams& other) const {
            
            return this->UniqueIndex == other.UniqueIndex
               && this->TexBufferWidth == other.TexBufferWidth
               && this->TexBufferHeight == other.TexBufferHeight
               && this->TexTarget == other.TexTarget
               && this->TexMagFilter == other.TexMagFilter
               && this->TexMinFilter == other.TexMinFilter
               && this->TexMipLvl == other.TexMipLvl
               && this->TexPixelInternalFormat == other.TexPixelInternalFormat
               && this->TexPixelFormat == other.TexPixelFormat
               && this->TexPixelType == other.TexPixelType
               && this->TexWrapMode == other.TexWrapMode;
         }
        
		};
	}
}

namespace std
{
   using namespace Graphics::Texture;
   template<>
   struct hash<TexParams>
   {
      std::size_t operator()(const TexParams& k) const
      {
         return hash<size_t>()(k.UniqueIndex)
         ^  hash<int32_t>()(k.TexBufferWidth)
         ^ hash<int32_t>()(k.TexBufferHeight)
         ^ hash<int32_t>()(k.TexTarget)
         ^ hash<int32_t>()(k.TexMagFilter)
         ^ hash<int32_t>()(k.TexMinFilter)
         ^ hash<int32_t>()(k.TexMipLvl)
         ^ hash<int32_t>()(k.TexPixelInternalFormat)
         ^ hash<int32_t>()(k.TexPixelFormat)
         ^ hash<int32_t>()(k.TexPixelType)
         ^ hash<int32_t>()(k.TexWrapMode);
      }
   };
}

