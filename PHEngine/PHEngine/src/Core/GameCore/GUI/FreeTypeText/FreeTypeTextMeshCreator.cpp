#include "FreeTypeTextMeshCreator.h"

#include "Core/GameCore/DataProviders/GeneralSystemSettingsDataProvider.h"
#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "FreeTypeFont.h"
#include "FreeTypeTextFieldProxy.h"

#include <stdio.h>

#include <algorithm>
#include <vector>

using namespace EngineCore::DataProviders;

namespace EngineCore::GUI {
FreeTypeTextMeshCreator::FreeTypeTextMeshCreator()
{
}

void FreeTypeTextMeshCreator::recalculateVertices(
    const std::string& text, float x, float y, int width, int height, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas)
{
    const auto windowWidth = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowWidth();
    const auto windowHeight = GeneralSystemSettingsDataProvider::GetInstance()->GetWindowHeight();

    _sx = 2.0 / windowWidth;
    _sy = 2.0 / windowHeight;
    _coords.clear(); // case there are any existing coords

    // Break the text into individual words
    std::vector<std::string> words = splitText(text);
    auto _pixelSize = 12;
    auto _alignment = eFontFlags::CenterAligned;

    std::vector<std::string> lines;
    int widthRemaining = width;
    int spaceWidth = calcWidth(" ", ftFontAtlas);
    int indent = _pixelSize;

    // Create lines from our text, each containing the maximum amount of words we can fit within the given width
    std::string curLine = "";
    for (std::string word : words) {
        int wordWidth = calcWidth(word, ftFontAtlas);

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

        recalculateVertices(line, x + indent, y, ftFontAtlas);
        y += (ftFontAtlas->GetFontFace()->getFaceHandle()->size->metrics.height >> 6);
        indent = 0;
    }
}

void FreeTypeTextMeshCreator::recalculateVertices(
    const std::string& text, float x, float y, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas)
{
    // Coordinates passed in should specify where to start drawing from the top left of the text,
    // but FreeType starts drawing from the bottom-right, therefore move down one line
    y += ftFontAtlas->GetFontFace()->getFaceHandle()->size->metrics.height >> 6;

    // Calculate alignment (if applicable)
    int textWidth = calcWidth(text, ftFontAtlas); // temp
    auto _alignment = eFontFlags::CenterAligned;
    if (_alignment == eFontFlags::CenterAligned)
        x -= textWidth / 2.0;
    else if (_alignment == eFontFlags::RightAligned)
        x -= textWidth;

    // Normalize window coordinates
    x = -1 + x * _sx;
    y = 1 - y * _sy;

    int atlasWidth = ftFontAtlas->getAtlasWidth();
    int atlasHeight = ftFontAtlas->getAtlasHeight();

    FreeTypeFontAtlas::Character* chars = ftFontAtlas->getCharInfo();

    for (const char* p = text.c_str(); *p; ++p) {
        float x2 = x + chars[*p].bitmapLeft * _sx; // scaled x coord
        float y2 = -y - chars[*p].bitmapTop * _sy; // scaled y coord
        float w = chars[*p].bitmapWidth * _sx; // scaled width of character
        float h = chars[*p].bitmapHeight * _sy; // scaled height of character

        // Calculate kerning value
        FT_Vector kerning;
        FT_Get_Kerning(
            ftFontAtlas->GetFontFace()->getFaceHandle(), // font face handle
            *p, // left glyph
            *(p + 1), // right glyph
            FT_KERNING_DEFAULT, // kerning mode
            &kerning); // variable to store kerning value

        // Advance cursor to start of next character
        x += (chars[*p].advanceX + (kerning.x >> 6)) * _sx;
        y += chars[*p].advanceY * _sy;

        // Skip glyphs with no pixels (e.g. spaces)
        if (!w || !h)
            continue;

        _coords.push_back(
            glm::vec4(
                x2, // window x
                -y2, // window y
                chars[*p].xOffset, // texture atlas x offset
                0)); // texture atlas y offset

        _coords.push_back(glm::vec4(x2 + w, -y2, chars[*p].xOffset + chars[*p].bitmapWidth / atlasWidth, 0));

        _coords.push_back(glm::vec4(x2, -y2 - h, chars[*p].xOffset, chars[*p].bitmapHeight / atlasHeight));

        _coords.push_back(glm::vec4(x2 + w, -y2, chars[*p].xOffset + chars[*p].bitmapWidth / atlasWidth, 0));

        _coords.push_back(glm::vec4(x2, -y2 - h, chars[*p].xOffset, chars[*p].bitmapHeight / atlasHeight));

        _coords.push_back(
            glm::vec4(
                x2 + w, -y2 - h, chars[*p].xOffset + chars[*p].bitmapWidth / atlasWidth, chars[*p].bitmapHeight / atlasHeight));
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

int FreeTypeTextMeshCreator::calcWidth(const std::string& text, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas)
{
    int width = 0;
    FreeTypeFontAtlas::Character* chars = ftFontAtlas->getCharInfo();
    auto text_str = text.c_str();
    for (const char* p = text_str; *p; ++p) {
        width += chars[*p].advanceX;
    }

    return width;
}

std::vector<glm::vec4> FreeTypeTextMeshCreator::CreateTextMesh(
    std::shared_ptr<FreeTypeTextFieldProxy> textFieldProxy, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas)
{
    std::vector<glm::vec4> coords;
    if (textFieldProxy) {
        recalculateVertices(
            textFieldProxy->GetText(), 0, 0, textFieldProxy->GetLineWidth(), textFieldProxy->GetLineHeight(), ftFontAtlas);
        coords = _coords;
    }
    return coords;
}
} // namespace EngineCore::GUI
