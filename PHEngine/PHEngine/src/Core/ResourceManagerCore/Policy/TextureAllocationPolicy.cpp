#include "TextureAllocationPolicy.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/GraphicsCore/Texture/CubemapTexture.h"

namespace Resources
{
   
   template class TextureAllocationPolicy<std::string>;

   template <typename Model>
	std::shared_ptr<ITexture> TextureAllocationPolicy<Model>::AllocateMemory(Model& arg)
	{
		std::shared_ptr<ITexture> resultTexture;
		std::vector<std::string> pathToTextures = EngineUtility::Split(arg, ',');
		
		switch (pathToTextures.size())
		{
		case 1:
		{
			std::string absolutePath = std::move(EngineUtility::ConvertFromRelativeToAbsolutePath(arg));
			resultTexture = std::shared_ptr<ITexture>(LoadTexture2dFromFile(absolutePath));
			break;
		}
		case 6:
		{
			resultTexture = std::shared_ptr<ITexture>(LoadTextureCubeFromFile(pathToTextures));
			break;
		}
		default: throw  std::invalid_argument("Undefined count of files.");
		}
		return resultTexture;
	}

   template <typename Model>
	void TextureAllocationPolicy<Model>::DeallocateMemory(std::shared_ptr<ITexture> arg)
	{
		arg->CleanUp();
	}

   template <typename Model>
	ITexture* TextureAllocationPolicy<Model>::LoadTexture2dFromFile(Model& pathToFile)
	{
		return new Texture2d(pathToFile, new TextureAnisotropy(8.0f));
	}

   template <typename Model>
	ITexture* TextureAllocationPolicy<Model>::LoadTextureCubeFromFile(std::vector<Model>& pathToFiles)
	{
		std::vector<std::string> absolutePaths;

		for (auto it = pathToFiles.begin(); it != pathToFiles.end(); ++it)
		{
			std::string absolutePath = std::move(EngineUtility::ConvertFromRelativeToAbsolutePath(*it));
			absolutePaths.emplace_back(std::move(absolutePath));
		}

		return new CubemapTexture(absolutePaths);
	}

}
