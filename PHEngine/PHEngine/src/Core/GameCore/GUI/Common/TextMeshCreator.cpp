#include "TextMeshCreator.h"

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <algorithm>

namespace EngineCore {
TextMeshCreator::TextMeshCreator(const std::shared_ptr<FontMetaFile>& metaData)
    : mMetaData(metaData)
{
}

Word TextMeshCreator::CreateEmptyWord(const std::shared_ptr<TextFieldProxy>& textField) const
{
    return Word(textField->GetFontSize());
}

TextLine TextMeshCreator::CreateEmptyLine(const std::shared_ptr<TextFieldProxy>& textField) const
{
    return TextLine(mMetaData->GetSpaceWidth(), textField->GetFontSize(), textField->GetLineMaxWidth());
}

TextMeshData TextMeshCreator::CreateTextMesh(const std::shared_ptr<TextFieldProxy>& text)
{
    return CreateQuadVertices(text, CreateStructure(text));
}

std::vector<TextLine> TextMeshCreator::CreateStructure(const std::shared_ptr<TextFieldProxy>& textField)
{
    const auto& text = textField->GetText();

    std::vector<TextLine> resultTextLines;
    TextLine currentLine = CreateEmptyLine(textField);
    Word currentWord = CreateEmptyWord(textField);

    auto utf8_vector = EngineUtility::ExtractUtf8FromUnicodeString(text);
    std::vector<int32_t> utf8_codes;
    std::transform(utf8_vector.begin(), utf8_vector.end(), std::back_inserter(utf8_codes), [](const auto& utf8_char) {
        return EngineUtility::Utf8_To_Unicode(utf8_char);
    });

    // iterate all characters and accumulate words
    for (const auto& utf8_code : utf8_codes) {
        if (utf8_code == SPACE_UTF8_CODE) {
            if (!currentLine.IsEnoughSpaceForWord(currentWord)) {
                resultTextLines.emplace_back(std::move(currentLine));
                currentLine = CreateEmptyLine(textField);
                if (!currentLine.IsEnoughSpaceForWord(currentWord)) {
                    LogInfo(
                        "TextMeshCreator::CreateStructure => Word is extremely big, even to fit inside empty line. Text: ", text);
                }
            }
            currentLine.AddWord(std::move(currentWord));
            currentWord = CreateEmptyWord(textField);
        } else {
            currentWord.AddCharacter(mMetaData->GetCharacter(utf8_code));
        }
    }

    // add final word
    if (!currentLine.IsEnoughSpaceForWord(currentWord)) {
        resultTextLines.emplace_back(std::move(currentLine));
        currentLine = CreateEmptyLine(textField);
        if (!currentLine.IsEnoughSpaceForWord(currentWord)) {
            LogInfo("TextMeshCreator::CreateStructure => Word is extremely big, even to fit inside empty line. Text: ", text);
        }
    }

    currentLine.AddWord(std::move(currentWord));
    resultTextLines.emplace_back(std::move(currentLine));
    return resultTextLines;
}

TextMeshData TextMeshCreator::CreateQuadVertices(const std::shared_ptr<TextFieldProxy>& text, const std::vector<TextLine>& lines)
{
    float textWidth = 0.0f, textHeight = 0.0f;
    float curserX = 0.0f;
    float curserY = 0.0f;
    std::vector<float> vertices;
    std::vector<float> textureCoords;
    for (const auto& line : lines) {
        const auto& horizontalAlignment = text->GetTextHorizontalAlignment();
        const auto& lineMaxLengthInScreenCoords = line.GetMaxLength();
        if (eTextHorizontalAlignmentType::CENTER == horizontalAlignment) {
            curserX = (lineMaxLengthInScreenCoords * 0.5f) - (line.GetLineLength() * 0.5f);
        } else if (eTextHorizontalAlignmentType::LEFT == horizontalAlignment) {
            curserX = 0.0f;
        } else if (eTextHorizontalAlignmentType::RIGHT == horizontalAlignment) {
            curserX = lineMaxLengthInScreenCoords - line.GetLineLength();
        }
        const float fontSize = text->GetFontSize();

        for (const auto& word : line.GetWords()) {
            for (const auto& letter : word.GetCharacters()) {
                AddVerticesForCharacter(curserX, curserY, letter, fontSize, vertices);
                AddTexCoords(
                    textureCoords,
                    letter.GetxTextureCoord(),
                    letter.GetyTextureCoord(),
                    letter.GetXMaxTextureCoord(),
                    letter.GetYMaxTextureCoord());
                curserX += letter.GetxAdvance() * fontSize;
            }
            curserX += mMetaData->GetSpaceWidth() * fontSize;
        }
        textWidth = std::max(curserX, textWidth);
        curserX = 0;
        curserY += FontMetaFile::LINE_HEIGHT * fontSize;
    }
    textHeight = curserY;
    return TextMeshData(vertices, textureCoords, textWidth, textHeight);
}

void TextMeshCreator::AddVerticesForCharacter(
    const float curserX, const float curserY, TextCharacter character, const float fontSize, std::vector<float>& vertices)
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

void TextMeshCreator::AddVertices(std::vector<float>& vertices, const float x, const float y, const float maxX, const float maxY)
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

void TextMeshCreator::AddTexCoords(
    std::vector<float>& texCoords, const float x, const float y, const float maxX, const float maxY)
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
} // namespace EngineCore
