#pragma once

#include "Character.h"

#include <vector>

namespace EngineCore
{
    /**
     * During the loading of a text this represents one word in the text.
     */
    class Word
    {
        std::vector<Character> mCharacters;

        float mWidth;

        float mFontSize;

    public:
        /**
         * Create a new empty word.
         * @param fontSize - the font size of the text which this word is in.
         */
        Word(const float fontSize);

        /**
         * Adds a character to the end of the current word and increases the screen-space width of the word.
         * @param character - the character to be added.
         */
        void AddCharacter(const Character& character);

        /**
         * @return The list of characters in the word.
         */
        const std::vector<Character>& GetCharacters() const;

        /**
         * @return The width of the word in terms of screen size.
         */
        float GetWordWidth() const;
    };
}
