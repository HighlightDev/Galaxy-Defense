#pragma once

#include "Word.h"

#include <vector>

namespace EngineCore {
/**
 * Represents a line of text during the loading of a text.
 */
class TextLine {
    float mMaxWidth;

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
    TextLine(const float spaceWidth, const float fontSize, const float maxWidth);

    TextLine(const TextLine& line) = default;

    TextLine(TextLine&& line);

    TextLine& operator=(TextLine&& line);

    TextLine& operator=(const TextLine& line) = default;

    bool IsEnoughSpaceForWord(const Word& word) const;

    void AddWord(Word word);
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
    const std::vector<Word>& GetWords() const;
};
} // namespace EngineCore
