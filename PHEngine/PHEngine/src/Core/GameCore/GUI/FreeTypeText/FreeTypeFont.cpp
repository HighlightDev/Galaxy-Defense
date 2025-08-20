#include "FreeTypeFont.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <numeric>

namespace EngineCore::GUI {

std::once_flag FreeTypeFont::s_initFlag;
FT_Library FreeTypeFont::mFt;
bool FreeTypeFont::mFtInitialized = false;
bool FreeTypeFont::mLanguageMapInitialized = false;

FreeTypeFont::FreeTypeFont(const std::string& fontFile)
{
    // Initialize FreeType
    std::call_once(s_initFlag, [this]() {
        // Initialize FreeType library only once
        const auto error = FT_Init_FreeType(&FreeTypeFont::mFt);
        if (error) {
            throw std::runtime_error("Failed to initialize FreeType");
        } else {
            mFtInitialized = true;
        }
    });
    setFontFile(fontFile);
}

FreeTypeFont::~FreeTypeFont()
{
    if (mFaceInitialized) {
        FT_Done_Face(mFace);
        mFaceInitialized = false;
    }
}

void FreeTypeFont::CleanUp()
{
    if (mFtInitialized) {
        FT_Done_FreeType(mFt);
        mFtInitialized = false;
    }
}

const std::unordered_map<std::string, std::vector<uint32_t>>& FreeTypeFont::getLanguageCharMap()
{
    // Map language codes to sets of UTF-8 character codes
    static std::unordered_map<std::string, std::vector<uint32_t>> languageCharMap = {
        {"en", std::vector<uint32_t>(128 - 32)}, // English
        {"ru", std::vector<uint32_t>(0x44F - 0x410 + 1)}, // Russian
        {"zh", std::vector<uint32_t>(0x4E50 - 0x4E00 + 1)}, // Chinese
    };

    if (!mLanguageMapInitialized) {
        std::iota(languageCharMap["en"].begin(), languageCharMap["en"].end(), 32); // Fill with ASCII characters from 32 to 127
        std::iota(
            languageCharMap["ru"].begin(),
            languageCharMap["ru"].end(),
            0x410); // Fill with Russian characters from 0x410 to 0x44F
        std::iota(
            languageCharMap["zh"].begin(),
            languageCharMap["zh"].end(),
            0x4E00); // Fill with Chinese characters from 0x4E00 to 0x4E50

        mLanguageMapInitialized = true;
    }

    return languageCharMap;
}

void FreeTypeFont::setFontFile(const std::string& fontFile)
{
    // Check if the font file exists
    std::ifstream file(fontFile);
    if (!file) {
        throw std::runtime_error("Font file does not exist: " + fontFile);
    }
    file.close();

    // Clean up previous face if it exists
    if (mFaceInitialized) {
        FT_Done_Face(mFace);
        mFaceInitialized = false;
    }

    // Create a new font
    const auto error = FT_New_Face(
        FreeTypeFont::mFt, // FreeType instance handle
        fontFile.c_str(), // Font family to use
        0, // index of font (in case there are more than one in the file)
        &mFace); // font face handle

    if (error == FT_Err_Unknown_File_Format) {
        throw std::runtime_error("Failed to open font: unknown font format");
    } else if (error) {
        throw std::runtime_error("Failed to open font");
    }

    mFaceInitialized = true;
}

FT_Face FreeTypeFont::getFaceHandle()
{
    return mFace;
}
} // namespace EngineCore::GUI