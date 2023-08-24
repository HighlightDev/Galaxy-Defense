#include "ITexture.h"

namespace Graphics
{
	namespace Texture
	{
		ITexture::ITexture()
			: m_texDescriptor(-1)
		{
		}


		ITexture::~ITexture()
		{
		}

		bool ITexture::operator==(const ITexture& right) const
		{
			return this->m_texDescriptor == right.m_texDescriptor;
		}

	}
}