#pragma once

#include "FreeTypeFontAtlas.h"

#include <freetype/freetype.h>
#include <ft2build.h>
#include <glm/glm.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

#ifndef PI
#define PI 3.141596
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG 180.0 / PI;
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD PI / 180.0
#endif

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

    int32_t CalcWidth(const std::string& text, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);

    int32_t CalcHeight(std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);

private:
    // Used to scale x and y coords
    // Note: sx and sy are chosen so that one glyph pixel corresponds to one screen pixel

    std::vector<std::string> splitText(const std::string& text);
    // Returns the width (in pixels) of the std::string, given the current pixel size

    // Calculate vertices for a paragraph label

    void calculateVertices(
        std::vector<glm::vec2>& vertices,
        std::vector<glm::vec2>& texCoords,
        const std::string& text,
        float x,
        float y,
        int width,
        int height,
        std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);
    // Calculate vertices without regards to width or height boundaries
    void calculateVertices(
        std::vector<glm::vec2>& vertices,
        std::vector<glm::vec2>& texCoords,
        const std::string& text,
        float x,
        float y,
        std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);
};

} // namespace EngineCore::GUI
