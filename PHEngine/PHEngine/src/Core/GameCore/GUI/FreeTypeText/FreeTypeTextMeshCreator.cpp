#include "FreeTypeTextMeshCreator.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "FreeTypeFont.h"
#include "FreeTypeTextFieldProxy.h"

#include <stdio.h>

#include <algorithm>
#include <vector>

using namespace EngineCore::DataProviders;

namespace EngineCore::GUI {

void FreeTypeTextMeshCreator::calculateVertices(
    std::vector<glm::vec2>& vertices,
    std::vector<glm::vec2>& texCoords,
    const std::string& text,
    float x,
    float y,
    const int width,
    const int height,
    std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas,
    const eTextHorizontalAlignmentType alignment,
    const int32_t fontSize)
{
    // Break the text into individual words
    std::vector<std::string> words = splitText(text);

    std::vector<std::string> lines;
    int widthRemaining = width;
    const int spaceWidth = CalcWidth(" ", ftFontAtlas);

    // todo
    int _flags = eFontFlags::WordWrap;
    int indent = (_flags & eFontFlags::Indented) && alignment != eTextHorizontalAlignmentType::CENTER ? fontSize : 0;

    // Create lines from our text, each containing the maximum amount of words we can fit within the given width
    std::string curLine = "";
    for (std::string word : words) {
        int wordWidth = CalcWidth(word, ftFontAtlas);

        if (wordWidth - spaceWidth > widthRemaining && width /* make sure there is a width specified */) {

            // If we have passed the given width, add this line to our collection and start a new line
            lines.push_back(curLine);
            widthRemaining = width - wordWidth;
            curLine = "";

            // Start next line with current word
            curLine.append(word);
        } else {
            // Otherwise, add this word to the current line
            curLine.append(word);
            widthRemaining = widthRemaining - wordWidth;
        }
    }

    // Add the last line to lines
    if (curLine != "")
        lines.push_back(curLine);

    // Print each line, increasing the y value as we go
    float startY = y - (ftFontAtlas->GetFontFace()->getFaceHandle()->size->metrics.height >> 6);
    for (std::string line : lines) {
        // If we go past the specified height, stop drawing
        if (y - startY > height && height)
            break;

        calculateVertices(vertices, texCoords, line, x + indent, y, ftFontAtlas, alignment);
        y += (ftFontAtlas->GetFontFace()->getFaceHandle()->size->metrics.height >> 6);
        indent = 0;
    }
}

void FreeTypeTextMeshCreator::calculateVertices(
    std::vector<glm::vec2>& vertices,
    std::vector<glm::vec2>& texCoords,
    const std::string& text,
    float x,
    float y,
    std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas,
    const eTextHorizontalAlignmentType alignment)
{
    const auto windowWidth = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth();
    const auto windowHeight = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight();
    const auto _sx = 2.0 / windowWidth;
    const auto _sy = 2.0 / windowHeight;

    // Coordinates passed in should specify where to start drawing from the top left of the text,
    // but FreeType starts drawing from the bottom-right, therefore move down one line
    y += ftFontAtlas->GetFontFace()->getFaceHandle()->size->metrics.height >> 6;

    // Normalize window coordinates
    x = -1 + x * _sx;
    y = 1 - y * _sy;

    int atlasWidth = ftFontAtlas->getAtlasWidth();
    int atlasHeight = ftFontAtlas->getAtlasHeight();

    const auto& chars = ftFontAtlas->getCharInfo();

    const std::vector<uint32_t> codepoints = EngineUtility::Utf8_To_Unicode(text);

    for (size_t idx = 0; idx < codepoints.size(); ++idx) {
        uint32_t cp = codepoints[idx];
        if (!chars.count(cp)) {
            continue; // skip characters not in the font atlas
        }
        const auto& character = chars.at(cp);
        float x2 = x + character.bitmapLeft * _sx; // scaled x coord
        float y2 = -y - character.bitmapTop * _sy; // scaled y coord
        float w = character.bitmapWidth * _sx; // scaled width of character
        float h = character.bitmapHeight * _sy; // scaled height of character

        // Calculate kerning value
        FT_Vector kerning = {0, 0};
        if (idx + 1 < codepoints.size()) {
            FT_Get_Kerning(ftFontAtlas->GetFontFace()->getFaceHandle(), cp, codepoints[idx + 1], FT_KERNING_DEFAULT, &kerning);
        }

        // Advance cursor to start of next character
        x += (character.advanceX + (kerning.x >> 6)) * _sx;
        y += character.advanceY * _sy;

        // Skip glyphs with no pixels (e.g. spaces)
        if (!w || !h)
            continue;

        vertices.emplace_back(x2, -y2);
        vertices.emplace_back(x2 + w, -y2);
        vertices.emplace_back(x2, -y2 - h);
        vertices.emplace_back(x2 + w, -y2);
        vertices.emplace_back(x2, -y2 - h);
        vertices.emplace_back(x2 + w, -y2 - h);

        texCoords.emplace_back(character.xOffset, 0);
        texCoords.emplace_back(character.xOffset + character.bitmapWidth / atlasWidth, 0);
        texCoords.emplace_back(character.xOffset, character.bitmapHeight / atlasHeight);
        texCoords.emplace_back(character.xOffset + character.bitmapWidth / atlasWidth, 0);
        texCoords.emplace_back(character.xOffset, character.bitmapHeight / atlasHeight);
        texCoords.emplace_back(character.xOffset + character.bitmapWidth / atlasWidth, character.bitmapHeight / atlasHeight);
    }
}

std::vector<std::string> FreeTypeTextMeshCreator::splitText(const std::string& text)
{
    std::vector<std::string> words;
    int startPos = 0; // start position of current word
    int endPos = text.find(' '); // end position of current word

    if (endPos == -1) {
        // There is only one word, so return early
        words.push_back(text);
        return words;
    }

    // Find each word in the text (delimited by spaces) and add it to our std::vector of words
    while (endPos != std::string::npos) {
        words.push_back(text.substr(startPos, endPos - startPos + 1));
        startPos = endPos + 1;
        endPos = text.find(' ', startPos);
    }

    // Add last word
    words.push_back(text.substr(startPos, std::min(endPos, (int)text.size()) - startPos + 1));

    return words;
}

int32_t FreeTypeTextMeshCreator::CalcWidth(const std::string& text, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas)
{
    int32_t width = 0;
    const auto& chars = ftFontAtlas->getCharInfo();
    const std::vector<uint32_t> codepoints = EngineUtility::Utf8_To_Unicode(text);
    for (const uint32_t c : codepoints) {
        if (!chars.count(c)) {
            continue; // skip characters not in the font atlas
        }
        width += chars.at(c).advanceX;
    }

    return width;
}

glm::ivec2 FreeTypeTextMeshCreator::CalcTextScreenSpaceSize(
    std::shared_ptr<FreeTypeTextFieldProxy> textFieldProxy, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas)
{
    if (textFieldProxy) {
        const auto& text = textFieldProxy->GetText();
        if (text.empty()) {
            return {0, 0};
        }

        std::vector<std::string> words = splitText(text);

        const int32_t width = textFieldProxy->GetLineWidthHeight().x;
        int widthRemaining = width;
        const int spaceWidth = CalcWidth(" ", ftFontAtlas);

        int32_t linesCount = 0;
        std::string curLine = "";
        for (const auto& word : words) {
            const int wordWidth = CalcWidth(word, ftFontAtlas);
            if (wordWidth - spaceWidth > widthRemaining && width /* make sure there is a width specified */) {
                // If we have passed the given width
                ++linesCount;
                widthRemaining = width - wordWidth;
                curLine = "";
                curLine.append(word);
            } else {
                curLine.append(word);
                widthRemaining = widthRemaining - wordWidth;
            }
        }

        if (curLine != "") {
            ++linesCount;
        }

        const int height = ftFontAtlas->GetFontFace()->getFaceHandle()->size->metrics.height >> 6;
        return {CalcWidth(textFieldProxy->GetText(), ftFontAtlas), linesCount * height};
    }
    return {0, 0};
}

std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>> FreeTypeTextMeshCreator::CreateTextMesh(
    std::shared_ptr<FreeTypeTextFieldProxy> textFieldProxy, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas)
{
    if (textFieldProxy) {
        std::vector<glm::vec2> vertices;
        std::vector<glm::vec2> texCoords;
        calculateVertices(
            vertices,
            texCoords,
            textFieldProxy->GetText(),
            0,
            0,
            textFieldProxy->GetLineWidthHeight().x,
            textFieldProxy->GetLineWidthHeight().y,
            ftFontAtlas,
            textFieldProxy->GetTextHorizontalAlignment(),
            textFieldProxy->GetFontSize());
        return {vertices, texCoords};
    }
    return {};
}
} // namespace EngineCore::GUI
