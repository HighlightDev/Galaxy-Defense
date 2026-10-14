#pragma once

#include <freetype/freetype.h>
#include <ft2build.h>

#include <atomic>
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

    void CleanUp();

    static void UnloadFreeTypeFontLibrary();

    static const std::unordered_map<std::string, std::vector<uint32_t>>& getLanguageCharMap();

    const std::string& GetFontFileName() const;

private:
    static std::atomic_bool s_initFlag;

    static FT_Library mFt;

    FT_Face mFace;

    bool mFaceInitialized = false;

    static bool mLanguageMapInitialized;

    std::string mFontFile;
};
} // namespace EngineCore::GUI
