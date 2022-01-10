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
		template <typename TextureParamsModelType>
		static std::shared_ptr<ITexture> AllocateMemory(const TextureParamsModelType &arg)
		{
			std::shared_ptr<ITexture> resultTexture;
			std::vector<std::string> pathToTextures = EngineUtility::Split(arg, ',');

			switch (pathToTextures.size())
			{
			case 1:
			{
				resultTexture = std::shared_ptr<ITexture>(LoadTexture2dFromFile(arg));
				break;
			}
			case 6:
			{
				resultTexture = std::shared_ptr<ITexture>(LoadTextureCubeFromFile(pathToTextures));
				break;
			}
			default:
				throw std::invalid_argument("Undefined count of files.");
			}
			return resultTexture;
		}

		static void DeallocateMemory(std::shared_ptr<ITexture> arg)
		{
			arg->CleanUp();
		}

	private:
		template <typename TextureParamsModelType>
		static ITexture *LoadTexture2dFromFile(const TextureParamsModelType &pathToFile)
		{
			return new Texture2d(pathToFile, new TextureAnisotropy(8.0f));
		}

		template <typename TextureParamsModelType>
		static ITexture *LoadTextureCubeFromFile(const std::vector<TextureParamsModelType> &pathToFiles)
		{
			std::vector<std::string> absolutePaths;

			for (auto it = pathToFiles.begin(); it != pathToFiles.end(); ++it)
			{
				absolutePaths.emplace_back(std::move(*it));
			}

			return new CubemapTexture(absolutePaths);
		}
	};

}
