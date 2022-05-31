#include "Line.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{
    Line::Line(const float spaceWidth, const float fontSize, const float maxLength)
        : mSpaceSize(spaceWidth * fontSize),
          mMaxLength(maxLength), 
          mWords(),
          mCurrentLineLength(0.0f)
    {
    }

    /**
     * Attempt to add a word to the line. If the line can fit the word in
     * without reaching the maximum line length then the word is added and the
     * line length increased.
     *
     * @param word
     *            - the word to try to add.
     * @return {@code true} if the word has successfully been added to the line.
     */
    bool Line::TryToAddWord(const Word &word)
    {
        float additionalLength = word.GetWordWidth();
        additionalLength += !mWords.empty() ? mSpaceSize : 0;
        if (mCurrentLineLength + additionalLength <= mMaxLength)
        {
            mWords.push_back(word);
            mCurrentLineLength += additionalLength;
            return true;
        }

        assert(false);

        return false;
    }

    /**
     * @return The max length of the line.
     */
    float Line::GetMaxLength() const
    {
        return mMaxLength;
    }

    /**
     * @return The current screen-space length of the line.
     */
    float Line::GetLineLength() const
    {
        return mCurrentLineLength;
    }

    /**
     * @return The list of words in the line.
     */
    const std::vector<Word> &Line::GetWords() const
    {
        return mWords;
    }
}