#pragma once

namespace Graphics
{
	namespace Texture
	{
		/************************************************************************/
		/*								ITextureMipMapState                     */
		/************************************************************************/
		class ITextureMipMapState
		{

		public:
			ITextureMipMapState();

			virtual ~ITextureMipMapState();

			virtual void ExecuteTextureSampleFilteringInstructions() = 0;
		};

		/************************************************************************/
		/*								TextureMipMap                           */
		/************************************************************************/
		class TextureMipMap : public ITextureMipMapState
		{

		public:

			TextureMipMap();

			~TextureMipMap();

			virtual void ExecuteTextureSampleFilteringInstructions() override;
		};

		/************************************************************************/
		/*								TextureAnisotropy                       */
		/************************************************************************/
		class TextureAnisotropy : public ITextureMipMapState
		{
			float m_anisitropyLvl;
			float m_preferableAnisotropyLvl;

			bool bAnisotropyLvlChecked;

		public :

			TextureAnisotropy(float preferableAnisotropyLvl);

			~TextureAnisotropy();

			virtual void ExecuteTextureSampleFilteringInstructions() override;

		private:

			float GetSupportedAnisotropyLvl();
		};

	}
}

