#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"
#include "Core/GraphicsCore/Texture/TexParams.h"

using namespace Graphics::Texture;

namespace Resources
{

   template <typename Model>
	class TextureAllocationPolicy
	{
	public:

		static std::shared_ptr<ITexture> AllocateMemory(Model& arg);

		static void DeallocateMemory(std::shared_ptr<ITexture> arg);

	private:

		static ITexture* LoadTexture2dFromFile(Model& pathToFile);

		static ITexture* LoadTextureCubeFromFile(std::vector<Model>& pathToFiles);
	};

}

