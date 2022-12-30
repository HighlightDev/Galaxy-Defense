#include "TextLine.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{
    TextLine::TextLine(const float spaceWidth, const float fontSize, const float maxWidth)
        : mSpaceSize(spaceWidth * fontSize),
          mMaxWidth(maxWidth),
          mWords(),
          mCurrentLineLength(0.0f)
    {
    }

    TextLine::TextLine(TextLine &&line)
        : mSpaceSize(line.mSpaceSize),
          mMaxWidth(line.mMaxWidth),
          mWords(std::move(line.mWords)),
          mCurrentLineLength(line.mCurrentLineLength)
    {
    }

    TextLine &TextLine::operator=(TextLine &&line)
    {
        if (this != &line)
        {
            mSpaceSize = line.mSpaceSize;
            mMaxWidth = line.mMaxWidth;
            mWords = std::move(line.mWords);
            mCurrentLineLength = line.mCurrentLineLength;
        }

        return *this;
    }

    bool TextLine::IsEnoughSpaceForWord(const Word &word) const
    {
        const auto wordWithSpaceWidth = word.GetWordWidth() + !mWords.empty() ? mSpaceSize : 0;
        return (mCurrentLineLength + wordWithSpaceWidth) <= mMaxWidth;
    }

    void TextLine::AddWord(Word word)
    {
        const auto wordWithSpaceWidth = word.GetWordWidth() + !mWords.empty() ? mSpaceSize : 0;
        mWords.emplace_back(std::move(word));
        mCurrentLineLength += wordWithSpaceWidth;
    }

    /**
     * @return The max length of the line.
     */
    float TextLine::GetMaxLength() const
    {
        return mMaxWidth;
    }

    /**
     * @return The current screen-space length of the line.
     */
    float TextLine::GetLineLength() const
    {
        return mCurrentLineLength;
    }

    /**
     * @return The list of words in the line.
     */
    const std::vector<Word> &TextLine::GetWords() const
    {
        return mWords;
    }
}