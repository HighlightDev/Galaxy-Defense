#include "TextMeshCreator.h"

#include "GUIText.h"

namespace EngineCore
{
	TextMeshCreator::TextMeshCreator(const std::string &fontFilePath)
		: mMetaData(FontMetaFile(fontFilePath))
	{
	}

	TextMeshCreator::TextMeshCreator(const FontMetaFile &metaData)
		: mMetaData(metaData)
	{
	}

	TextMeshData TextMeshCreator::CreateTextMesh(GUIText &text)
	{
		return CreateQuadVertices(text, CreateStructure(text));
	}

	std::vector<Line> TextMeshCreator::CreateStructure(GUIText &text)
	{
		const auto &chars = text.GetTextString();
		std::vector<Line> lines;
		Line currentLine(mMetaData.GetSpaceWidth(), text.GetFontSize(), text.GetMaxLineSize());
		Word currentWord(text.GetFontSize());
		for (const auto &c : chars)
		{
			const int32_t ascii = (int32_t)c;
			if (ascii == SPACE_ASCII)
			{
				const bool bIsAdded = currentLine.TryToAddWord(currentWord);
				if (!bIsAdded)
				{
					lines.emplace_back(currentLine);
					currentLine = Line(mMetaData.GetSpaceWidth(), text.GetFontSize(), text.GetMaxLineSize());
					currentLine.TryToAddWord(currentWord);
				}
				currentWord = Word(text.GetFontSize());
				continue;
			}
			Character character = mMetaData.GetCharacter(ascii);
			currentWord.AddCharacter(character);
		}
		CompleteStructure(lines, currentLine, currentWord, text);
		return lines;
	}

	void TextMeshCreator::CompleteStructure(std::vector<Line> &lines, Line &currentLine, const Word &currentWord, GUIText &text)
	{
		const bool bAdded = currentLine.TryToAddWord(currentWord);
		if (!bAdded)
		{
			lines.emplace_back(currentLine);
			Line newLine(mMetaData.GetSpaceWidth(), text.GetFontSize(), text.GetMaxLineSize());
			newLine.TryToAddWord(currentWord);
			lines.emplace_back(newLine);
		}
		lines.emplace_back(currentLine);
	}

	TextMeshData TextMeshCreator::CreateQuadVertices(GUIText &text, const std::vector<Line> &lines)
	{
		text.SetNumberOfLines(lines.size());
		float curserX = 0.0f;
		float curserY = 0.0f;
		std::vector<float> vertices;
		std::vector<float> textureCoords;
		for (const auto &line : lines)
		{
			if (text.IsCentered())
			{
				const auto &lineMaxLengthInScreenCoords = line.GetMaxLength();
				curserX = (lineMaxLengthInScreenCoords * 0.5f) - (line.GetLineLength() * 0.5f);
			}

			for (const auto &word : line.GetWords())
			{
				for (const auto &letter : word.GetCharacters())
				{
					AddVerticesForCharacter(curserX, curserY, letter, text.GetFontSize(), vertices);
					AddTexCoords(textureCoords, letter.GetxTextureCoord(), letter.GetyTextureCoord(),
								 letter.GetXMaxTextureCoord(), letter.GetYMaxTextureCoord());
					curserX += letter.GetxAdvance() * text.GetFontSize();
				}
				curserX += mMetaData.GetSpaceWidth() * text.GetFontSize();
			}
			curserX = 0;
			curserY += LINE_HEIGHT * text.GetFontSize();
		}
		return TextMeshData(vertices, textureCoords);
	}

	void TextMeshCreator::AddVerticesForCharacter(const float curserX, const float curserY, Character character, const float fontSize,
												  std::vector<float> &vertices)
	{
		const float x = curserX + (character.GetxOffset() * fontSize);
		const float y = curserY + (character.GetyOffset() * fontSize);
		const float maxX = x + (character.GetSizeX() * fontSize);
		const float maxY = y + (character.GetSizeY() * fontSize);
		const float properX = (2 * x) - 1;
		const float properY = (-2 * y) + 1;
		const float properMaxX = (2 * maxX) - 1;
		const float properMaxY = (-2 * maxY) + 1;
		AddVertices(vertices, properX, properY, properMaxX, properMaxY);
	}

	void TextMeshCreator::AddVertices(std::vector<float> &vertices, const float x, const float y, const float maxX, const float maxY)
	{
		vertices.emplace_back(x);
		vertices.emplace_back(y);
		vertices.emplace_back(x);
		vertices.emplace_back(maxY);
		vertices.emplace_back(maxX);
		vertices.emplace_back(maxY);
		vertices.emplace_back(maxX);
		vertices.emplace_back(maxY);
		vertices.emplace_back(maxX);
		vertices.emplace_back(y);
		vertices.emplace_back(x);
		vertices.emplace_back(y);
	}

	void TextMeshCreator::AddTexCoords(std::vector<float> &texCoords, const float x, const float y, const float maxX, const float maxY)
	{
		texCoords.emplace_back(x);
		texCoords.emplace_back(y);
		texCoords.emplace_back(x);
		texCoords.emplace_back(maxY);
		texCoords.emplace_back(maxX);
		texCoords.emplace_back(maxY);
		texCoords.emplace_back(maxX);
		texCoords.emplace_back(maxY);
		texCoords.emplace_back(maxX);
		texCoords.emplace_back(y);
		texCoords.emplace_back(x);
		texCoords.emplace_back(y);
	}
}
