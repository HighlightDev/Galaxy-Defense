#include "FreeTypeFont.h"

#include <exception>
#include <fstream>
namespace EngineCore::GUI {

std::once_flag FreeTypeFont::s_initFlag;

FreeTypeFont::FreeTypeFont(const std::string& fontFile)
{
    // Initialize FreeType
    std::call_once(s_initFlag, [this]() {
        // Initialize FreeType library only once
        const auto error = FT_Init_FreeType(&mFt);
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
    if (mFtInitialized) {
        FT_Done_FreeType(mFt);
        mFtInitialized = false;
    }
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
        mFt, // FreeType instance handle
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