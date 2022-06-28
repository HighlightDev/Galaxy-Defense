#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Core/GraphicsCore/Texture/ITexture.h"
#include "Core/GraphicsCore/Texture/Texture2d.h"
#include "Core/GraphicsCore/Texture/TexParams.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/GraphicsCore/Texture/CubemapTexture.h"

using namespace Graphics::Texture;

namespace Resources
{
	struct TextureAllocationPolicy
	{
		static std::shared_ptr<ITexture> AllocateMemory(const std::string &arg);

		static void DeallocateMemory(std::shared_ptr<ITexture> arg);

	private:
		static std::shared_ptr<ITexture> LoadTexture2dFromFile(const std::string &arg);

		static std::shared_ptr<ITexture> LoadTextureCubeFromFile(const std::vector<std::string> &pathToFiles);
	};

}
