#include "FontType.h"

namespace EngineCore
{
	FontType::FontType(const int32_t textureAtlas, const std::string &pathToFontFile)
		: mTextureAtlas(textureAtlas),
		  mTextMeshCreator(TextMeshCreator(pathToFontFile))
	{
	}

	int32_t FontType::GetTextureAtlas() const
	{
		return mTextureAtlas;
	}

	TextMeshData FontType::CreateTextMeshData(GUIText &text)
	{
		return mTextMeshCreator.CreateTextMesh(text);
	}
}