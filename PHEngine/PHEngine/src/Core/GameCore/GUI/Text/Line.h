#pragma once

#include <vector>

#include "Word.h"

namespace EngineCore
{
    /**
     * Represents a line of text during the loading of a text.
     */
    class Line
    {

        float mMaxLength;

        float mSpaceSize;

        std::vector<Word> mWords;

        float mCurrentLineLength;

    public:
        /**
         * Creates an empty line.
         *
         * @param spaceWidth
         *            - the screen-space width of a space character.
         * @param fontSize
         *            - the size of font being used.
         * @param maxLength
         *            - the screen-space maximum length of a line.
         */
        Line(const float spaceWidth, const float fontSize, const float maxLength);

        /**
         * Attempt to add a word to the line. If the line can fit the word in
         * without reaching the maximum line length then the word is added and the
         * line length increased.
         *
         * @param word
         *            - the word to try to add.
         * @return {@code true} if the word has successfully been added to the line.
         */
        bool TryToAddWord(const Word &word);
        /**
         * @return The max length of the line.
         */
        float GetMaxLength() const;

        /**
         * @return The current screen-space length of the line.
         */
        float GetLineLength() const;

        /**
         * @return The list of words in the line.
         */
        const std::vector<Word> &GetWords() const;
    };
}
