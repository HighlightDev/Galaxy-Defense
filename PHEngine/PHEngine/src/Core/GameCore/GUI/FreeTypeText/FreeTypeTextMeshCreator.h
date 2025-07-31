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
    FreeTypeTextMeshCreator();

    ~FreeTypeTextMeshCreator();

private:
    // Used to scale x and y coords
    // Note: sx and sy are chosen so that one glyph pixel corresponds to one screen pixel
    float _sx;
    float _sy;

    std::vector<glm::vec4> _coords; // Holds texture coordinates for each glyph in the text

    std::vector<std::string> splitText(const std::string& text);
    // Returns the width (in pixels) of the std::string, given the current pixel size
    int calcWidth(const std::string& text, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);

    // Calculate vertices for a paragraph label

    void recalculateVertices(
        const std::string& text, float x, float y, int width, int height, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);
    // Calculate vertices without regards to width or height boundaries
    void recalculateVertices(const std::string& text, float x, float y, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);

public:
    /*  x offset in window coordinates
     *  y offset in window coordinates
     *  glyph x offset in texture coordinates
     *  glyph y offset in texture coordinates
     */
    std::vector<glm::vec4>
    CreateTextMesh(std::shared_ptr<FreeTypeTextFieldProxy> textFieldProxy, std::shared_ptr<FreeTypeFontAtlas> ftFontAtlas);
};

} // namespace EngineCore::GUI
