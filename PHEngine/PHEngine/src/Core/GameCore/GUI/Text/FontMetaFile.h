#pragma once

#include "Character.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <optional>

namespace EngineCore
{
    /**
     * Provides functionality for getting the values from a font file.
     *
     */
    class FontMetaFile
    {
        static constexpr int32_t PAD_TOP = 0;
        static constexpr int32_t PAD_LEFT = 1;
        static constexpr int32_t PAD_BOTTOM = 2;
        static constexpr int32_t PAD_RIGHT = 3;

        static constexpr int32_t DESIRED_PADDING = 3;

        static constexpr float LINE_HEIGHT = 0.03f;
        static constexpr int32_t SPACE_ASCII = 32;

        static constexpr char SPLITTER = ' ';
        static constexpr char NUMBER_SEPARATOR = ',';

        float mAspectRatio;
        float mVerticalPerPixelSize;
        float mHorizontalPerPixelSize;
        float mSpaceWidth;
        std::vector<int32_t> mPadding;
        int32_t mPaddingWidth;
        int32_t mPaddingHeight;

        std::unordered_map<int32_t, Character> mMetaData;
        std::unordered_map<std::string, std::string> mValues;

        std::list<std::string> mFileSrc;

    public:
        /**
         * Opens a font file in preparation for reading.
         *
         * @param pathToFile
         *            - the font file.
         */
        FontMetaFile(const std::string &pathToFile);

        float GetSpaceWidth() const;

        Character GetCharacter(const int32_t ascii) const;

        /**
         * Read in the next line and store the variable values.
         *
         * @return {@code true} if the end of the file hasn't been reached.
         */
        bool TryProcessNextLine();

        /**
         * Gets the {@code int} value of the variable with a certain name on the
         * current line.
         *
         * @param variable
         *            - the name of the variable.
         * @return The value of the variable.
         */
        int32_t GetValueOfVariable(const std::string &variable) const;

        /**
         * Gets the array of ints associated with a variable on the current line.
         *
         * @param variable
         *            - the name of the variable.
         * @return The int array of values associated with the variable.
         */
        std::vector<int32_t> GetValuesOfVariable(const std::string &variable) const;

        /**
         * Opens the font file, ready for reading.
         *
         * @param file
         *            - the font file.
         */
        void OpenFile(const std::string &pathToFile);

        /**
         * Loads the data about how much padding is used around each character in
         * the texture atlas.
         */
        void LoadPaddingData();

        /**
         * Loads information about the line height for this font in pixels, and uses
         * this as a way to find the conversion rate between pixels in the texture
         * atlas and screen-space.
         */
        void LoadLineSizes();

        /**
         * Loads in data about each character and stores the data in the
         * {@link Character} class.
         *
         * @param imageWidth
         *            - the width of the texture atlas in pixels.
         */
        void LoadCharacterData(const int32_t imageWidth);

        /**
         * Loads all the data about one character in the texture atlas and converts
         * it all from 'pixels' to 'screen-space' before storing. The effects of
         * padding are also removed from the data.
         *
         * @param imageSize
         *            - the size of the texture atlas in pixels.
         * @return The data about the character.
         */
        std::optional<Character> TryLoadCharacter(const int32_t imageSize);
    };
}