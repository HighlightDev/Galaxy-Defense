#include "TextureMipMapState.h"

#include <gl/glew.h>
#include <algorithm>

namespace Graphics
{
	namespace Texture
	{
		/************************************************************************/
		/*								ITextureMipMapState                     */
		/************************************************************************/
		ITextureMipMapState::ITextureMipMapState()
		{
		}

		ITextureMipMapState::~ITextureMipMapState()
		{
		}

		/************************************************************************/
		/*								TextureMipMap                           */
		/************************************************************************/
		TextureMipMap::TextureMipMap()
		{
		}

		TextureMipMap::~TextureMipMap()
		{
		}

		void TextureMipMap::ExecuteTextureSampleFilteringInstructions()
		{
			// TODO : Not implemented yet
		}

		/************************************************************************/
		/*								TextureAnisotropy                       */
		/************************************************************************/
		TextureAnisotropy::TextureAnisotropy(float preferableAnisotropyLvl)
			: m_anisitropyLvl(0.0f)
			, m_preferableAnisotropyLvl(preferableAnisotropyLvl)
			, bAnisotropyLvlChecked(false)
		{
		}

		TextureAnisotropy::~TextureAnisotropy()
		{
		}

		void TextureAnisotropy::ExecuteTextureSampleFilteringInstructions()
		{
			if (GetSupportedAnisotropyLvl() > 0.0f)
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GetSupportedAnisotropyLvl());
		}

		float TextureAnisotropy::GetSupportedAnisotropyLvl()
		{
			if (!bAnisotropyLvlChecked)
			{
				float resultLvl = 0.0f;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &resultLvl);
				
				if (resultLvl > 0.0f)
					m_anisitropyLvl = std::min(m_preferableAnisotropyLvl, resultLvl);

				bAnisotropyLvlChecked = true;
			}

			return m_anisitropyLvl;
		}
	}
}