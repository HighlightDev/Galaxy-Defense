#pragma once

#include <freetype/freetype.h>
#include <ft2build.h>

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace EngineCore::GUI {
class FreeTypeFont {
public:
    FreeTypeFont(const std::string& fontFile);
    ~FreeTypeFont();

    void setFontFile(const std::string& fontFile);

    FT_Face getFaceHandle();

    static void CleanUp();

    static const std::unordered_map<std::string, std::vector<uint32_t>>& getLanguageCharMap();

private:
    static std::once_flag s_initFlag;

    static FT_Library mFt;

    static bool mFtInitialized;

    FT_Face mFace;

    bool mFaceInitialized = false;

    static bool mLanguageMapInitialized;
};
} // namespace EngineCore::GUI
