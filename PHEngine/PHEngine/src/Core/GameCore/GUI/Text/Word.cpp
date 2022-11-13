#include "Word.h"

namespace EngineCore
{
    Word::Word(const float fontSize)
        : mCharacters(),
          mWidth(0.0f),
          mFontSize(fontSize)
    {
    }

    /**
     * Adds a character to the end of the current word and increases the screen-space width of the word.
     * @param character - the character to be added.
     */
    void Word::AddCharacter(const TextCharacter &character)
    {
        mCharacters.push_back(character);
        mWidth += character.GetxAdvance() * mFontSize;
    }

    /**
     * @return The list of characters in the word.
     */
    const std::vector<TextCharacter> &Word::GetCharacters() const
    {
        return mCharacters;
    }

    /**
     * @return The width of the word in terms of screen size.
     */
    float Word::GetWordWidth() const
    {
        return mWidth;
    }
}