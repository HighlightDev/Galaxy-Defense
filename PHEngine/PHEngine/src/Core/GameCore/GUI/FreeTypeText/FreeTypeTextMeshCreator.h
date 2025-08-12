#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "FreeTypeFontAtlas.h"

#include <freetype/freetype.h>
#include <ft2build.h>
#include <glm/glm.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace EngineCore::GUI {

class FreeTypeTextFieldProxy;

class FreeTypeTextMeshCreator {
public:
    // Ctor takes a pointer to a font face
    FreeTypeTextMeshCreator() = default;

    ~FreeTypeTextMeshCreator() = default;

    /*  x offset in window coordinates
     *  y offset in window coordinates
     *  glyph x offset in texture coordinates
     *  glyph y offset in texture coordinates
     */
    std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>>
    CreateTextMesh(std::shared_ptr<FreeTypeTextFieldProxy> textFieldProxy, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);

    static int32_t CalcWidth(const std::string& text, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);

    static int32_t CalcHeight(std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);

private:
    static std::vector<std::string> splitText(const std::string& text);

    void calculateVertices(
        std::vector<glm::vec2>& vertices,
        std::vector<glm::vec2>& texCoords,
        const std::string& text,
        float x,
        float y,
        const int width,
        const int height,
        std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas,
        const eTextHorizontalAlignmentType alignment,
        const int32_t fontSize);
    // Calculate vertices without regards to width or height boundaries
    void calculateVertices(
        std::vector<glm::vec2>& vertices,
        std::vector<glm::vec2>& texCoords,
        const std::string& text,
        float x,
        float y,
        std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas,
        const eTextHorizontalAlignmentType alignment);
};

} // namespace EngineCore::GUI
