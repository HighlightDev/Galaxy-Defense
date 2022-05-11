#pragma once

#include "Line.h"
#include "FontMetaFile.h"
#include "TextMeshData.h"

#include <vector>

namespace EngineCore
{
	class GUIText;

	class TextMeshCreator
	{
		static constexpr float LINE_HEIGHT = 0.03f;

		static constexpr int32_t SPACE_ASCII = 32;

		FontMetaFile mMetaData;

	public:
		TextMeshCreator(const std::string &fontFilePath);

		TextMeshData CreateTextMesh(GUIText &text);

		std::vector<Line> CreateStructure(GUIText &text);

		void CompleteStructure(std::vector<Line> &lines, Line &currentLine, const Word &currentWord, GUIText &text);

		TextMeshData CreateQuadVertices(GUIText &text, const std::vector<Line> &lines);

		void AddVerticesForCharacter(const float curserX, const float curserY, Character character, const float fontSize,
									 std::vector<float> &vertices);

		void AddVertices(std::vector<float> &vertices, const float x, const float y, const float maxX, const float maxY);

		void AddTexCoords(std::vector<float> &texCoords, const float x, const float y, const float maxX, const float maxY);
	};
}
