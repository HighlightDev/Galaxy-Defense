#pragma once

#include "Line.h"
#include "FontMetaFile.h"
#include "TextMeshData.h"
#include "Core/GameCore/GUI/Text/TextFieldProxy.h"

#include <vector>
#include <memory>

using namespace Graphics;

namespace EngineCore
{
	class TextMeshCreator
	{
		static constexpr float LINE_HEIGHT = 0.03f;

		static constexpr int32_t SPACE_ASCII = 32;

		std::shared_ptr<FontMetaFile> mMetaData;

	public:
		TextMeshCreator(const std::shared_ptr<FontMetaFile>& metaData);

		TextMeshData CreateTextMesh(const std::shared_ptr<TextFieldProxy> &text);

		std::vector<Line> CreateStructure(const std::shared_ptr<TextFieldProxy> &text);

		void CompleteStructure(std::vector<Line> &lines, Line &currentLine, const Word &currentWord, const std::shared_ptr<TextFieldProxy> &text);

		TextMeshData CreateQuadVertices(const std::shared_ptr<TextFieldProxy> &text, const std::vector<Line> &lines);

		void AddVerticesForCharacter(const float curserX, const float curserY, Character character, const float fontSize,
									 std::vector<float> &vertices);

		void AddVertices(std::vector<float> &vertices, const float x, const float y, const float maxX, const float maxY);

		void AddTexCoords(std::vector<float> &texCoords, const float x, const float y, const float maxX, const float maxY);
	};
}
