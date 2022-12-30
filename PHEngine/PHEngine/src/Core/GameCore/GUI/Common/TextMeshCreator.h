#pragma once

#include "TextLine.h"
#include "FontMetaFile.h"
#include "TextMeshData.h"
#include "TextFieldProxy.h"

#include <vector>
#include <memory>

using namespace Graphics;

namespace EngineCore
{
	class TextMeshCreator
	{
		static constexpr int32_t SPACE_UTF8_CODE = 32;

		std::shared_ptr<FontMetaFile> mMetaData;

	public:
		TextMeshCreator(const std::shared_ptr<FontMetaFile>& metaData);

		TextMeshData CreateTextMesh(const std::shared_ptr<TextFieldProxy> &text);

		std::vector<TextLine> CreateStructure(const std::shared_ptr<TextFieldProxy> &text);

		TextMeshData CreateQuadVertices(const std::shared_ptr<TextFieldProxy> &text, const std::vector<TextLine> &lines);

		void AddVerticesForCharacter(const float curserX, const float curserY, TextCharacter character, const float fontSize,
									 std::vector<float> &vertices);

		void AddVertices(std::vector<float> &vertices, const float x, const float y, const float maxX, const float maxY);

		void AddTexCoords(std::vector<float> &texCoords, const float x, const float y, const float maxX, const float maxY);

		private:

		Word CreateEmptyWord(const std::shared_ptr<TextFieldProxy>& textField) const;

		TextLine CreateEmptyLine(const std::shared_ptr<TextFieldProxy>& textField) const;
	};
}
