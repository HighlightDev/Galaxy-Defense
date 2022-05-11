#pragma once

#include "TextMeshCreator.h"

namespace EngineCore
{
	class GUIText;

	class FontType
	{
		int32_t mTextureAtlas;

		TextMeshCreator mTextMeshCreator;

	public:
		/**
		 * Creates a new font and loads up the data about each character from the
		 * font file.
		 *
		 * @param textureAtlas
		 *            - the ID of the font atlas texture.
		 * @param fontFile
		 *            - the font file containing information about each character in
		 *            the texture atlas.
		 */
		FontType(const int32_t textureAtlas, const std::string &pathToFontFile);

		/**
		 * @return The font texture atlas.
		 */
		int32_t GetTextureAtlas() const;

		/**
		 * Takes in an unloaded text and calculate all of the vertices for the quads
		 * on which this text will be rendered. The vertex positions and texture
		 * coords and calculated based on the information from the font file.
		 *
		 * @param text
		 *            - the unloaded text.
		 * @return Information about the vertices of all the quads.
		 */
		TextMeshData CreateTextMeshData(GUIText &text);
	};
}
