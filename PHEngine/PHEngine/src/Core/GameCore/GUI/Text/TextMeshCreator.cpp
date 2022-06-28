#include "TextMeshCreator.h"

namespace EngineCore
{
	TextMeshCreator::TextMeshCreator(const std::shared_ptr<FontMetaFile> &metaData)
		: mMetaData(metaData)
	{
	}

	TextMeshData TextMeshCreator::CreateTextMesh(const std::shared_ptr<TextFieldProxy> &text)
	{
		return CreateQuadVertices(text, CreateStructure(text));
	}

	std::vector<Line> TextMeshCreator::CreateStructure(const std::shared_ptr<TextFieldProxy> &text)
	{
		const auto &chars = text->mText;
		std::vector<Line> lines;
		Line currentLine(mMetaData->GetSpaceWidth(), text->mFontSize, text->mLineMaxSize);
		Word currentWord(text->mFontSize);
		for (const auto &c : chars)
		{
			const int32_t ascii = (int32_t)c;
			if (ascii == SPACE_ASCII)
			{
				const bool bIsAdded = currentLine.TryToAddWord(currentWord);
				if (!bIsAdded)
				{
					lines.emplace_back(currentLine);
					currentLine = Line(mMetaData->GetSpaceWidth(), text->mFontSize, text->mLineMaxSize);
					currentLine.TryToAddWord(currentWord);
				}
				currentWord = Word(text->mFontSize);
				continue;
			}
			Character character = mMetaData->GetCharacter(ascii);
			currentWord.AddCharacter(character);
		}
		CompleteStructure(lines, currentLine, currentWord, text);
		return lines;
	}

	void TextMeshCreator::CompleteStructure(std::vector<Line> &lines, Line &currentLine, const Word &currentWord, const std::shared_ptr<TextFieldProxy> &text)
	{
		const bool bAdded = currentLine.TryToAddWord(currentWord);
		if (!bAdded)
		{
			lines.emplace_back(currentLine);
			Line newLine(mMetaData->GetSpaceWidth(), text->mFontSize, text->mLineMaxSize);
			newLine.TryToAddWord(currentWord);
			lines.emplace_back(newLine);
		}
		lines.emplace_back(currentLine);
	}

	TextMeshData TextMeshCreator::CreateQuadVertices(const std::shared_ptr<TextFieldProxy> &text, const std::vector<Line> &lines)
	{
		float textWidth = 0.0f, textHeight = 0.0f;
		float curserX = 0.0f;
		float curserY = 0.0f;
		std::vector<float> vertices;
		std::vector<float> textureCoords;
		for (const auto &line : lines)
		{
			if (text->mIsCenteredText)
			{
				const auto &lineMaxLengthInScreenCoords = line.GetMaxLength();
				curserX = (lineMaxLengthInScreenCoords * 0.5f) - (line.GetLineLength() * 0.5f);
			}
			const float fontSize = text->mFontSize;

			for (const auto &word : line.GetWords())
			{
				for (const auto &letter : word.GetCharacters())
				{
					AddVerticesForCharacter(curserX, curserY, letter, fontSize, vertices);
					AddTexCoords(textureCoords, letter.GetxTextureCoord(), letter.GetyTextureCoord(),
								 letter.GetXMaxTextureCoord(), letter.GetYMaxTextureCoord());
					curserX += letter.GetxAdvance() * fontSize;
				}
				curserX += mMetaData->GetSpaceWidth() * fontSize;
			}
			textWidth = std::max(curserX, textWidth);
			curserX = 0;
			curserY += LINE_HEIGHT * fontSize;
		}
		textHeight = curserY;
		return TextMeshData(vertices, textureCoords, textWidth, textHeight);
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
