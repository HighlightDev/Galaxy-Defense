#pragma once

#include <freetype/freetype.h>
#include <ft2build.h>

#include <string>
#include <mutex>

namespace GameCore::GUI {
class FreeTypeFont {
public:
    FreeTypeFont(const std::string& fontFile);
    ~FreeTypeFont();

    void setFontFile(const std::string& fontFile);

    FT_Face getFaceHandle();

private:
    static std::once_flag s_initFlag;

    FT_Library mFt;
    bool mFtInitialized = false;

    FT_Face mFace;
    bool mFaceInitialized = false;
};
} // namespace GameCore::GUI
