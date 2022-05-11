#include "FontMetaFile.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/IoCore/DisplayDeviceDataProvider.h"
#include "Core/IoCore/FileFacade.h"

using namespace IO;
using namespace EngineUtility;

namespace EngineCore
{
    FontMetaFile::FontMetaFile(const std::string &pathToFile)
        : mAspectRatio(0.0f),
          mVerticalPerPixelSize(0.0f),
          mHorizontalPerPixelSize(0.0f),
          mSpaceWidth(0.0f),
          mPadding(),
          mPaddingWidth(0.0f),
          mPaddingHeight(0.0f),
          mMetaData(),
          mValues(),
          mFileSrc()
    {
        mAspectRatio = DisplayDeviceDataProvider::GetInstance()->GetWidthToHeightRatio();
        OpenFile(pathToFile);
        LoadPaddingData();
        LoadLineSizes();
        const auto imageWidth = GetValueOfVariable("scaleW");
        LoadCharacterData(imageWidth);
    }

    float FontMetaFile::GetSpaceWidth() const
    {
        return mSpaceWidth;
    }

    Character FontMetaFile::GetCharacter(const int32_t ascii) const
    {
        assert(mMetaData.count(ascii));
        return mMetaData.at(ascii);
    }

    bool FontMetaFile::TryProcessNextLine()
    {
        if (mFileSrc.empty())
            return false;

        mValues.clear();

        const std::string &line = *mFileSrc.begin();
        const auto &splitedLine = Split(line, SPLITTER);

        for (const auto &word : splitedLine)
        {
            const auto &valuePairs = Split(word, '=');
            if (2 == valuePairs.size())
            {
                mValues.emplace(valuePairs[0], valuePairs[1]);
            }
        }

        mFileSrc.erase(mFileSrc.begin());

        return true;
    }

    int32_t FontMetaFile::GetValueOfVariable(const std::string &variable) const
    {
        assert(mValues.count(variable));
        return std::atoi(mValues.at(variable).c_str());
    }

    std::vector<int32_t> FontMetaFile::GetValuesOfVariable(const std::string &variable) const
    {
        assert(mValues.count(variable));
        const auto &numbersStrings = Split(mValues.at(variable), NUMBER_SEPARATOR);
        std::vector<int32_t> numbers;
        numbers.reserve(numbersStrings.size());

        std::for_each(numbersStrings.begin(), numbersStrings.end(), [&](const auto &valueStr)
                      { numbers.push_back(std::atoi(valueStr.c_str())); });

        return numbers;
    }

    void FontMetaFile::OpenFile(const std::string &pathToFile)
    {
        FileFacade fileReader(pathToFile);
        mFileSrc = fileReader.GetFileSrc();
    }

    void FontMetaFile::LoadPaddingData()
    {
        TryProcessNextLine();
        mPadding = GetValuesOfVariable("padding");
        mPaddingWidth = mPadding[PAD_LEFT] + mPadding[PAD_RIGHT];
        mPaddingHeight = mPadding[PAD_TOP] + mPadding[PAD_BOTTOM];
    }

    void FontMetaFile::LoadLineSizes()
    {
        TryProcessNextLine();
        const auto lineHeightPixels = GetValueOfVariable("lineHeight") - mPaddingHeight;
        mVerticalPerPixelSize = LINE_HEIGHT / (float)lineHeightPixels;
        mHorizontalPerPixelSize = mVerticalPerPixelSize / mAspectRatio;
    }

    void FontMetaFile::LoadCharacterData(const int32_t imageWidth)
    {
        TryProcessNextLine();
        TryProcessNextLine();
        while (TryProcessNextLine())
        {
            const auto &optResult = TryLoadCharacter(imageWidth);
            if (optResult)
            {
                mMetaData.emplace(optResult->GetId(), *optResult);
            }
        }
    }

    std::optional<Character> FontMetaFile::TryLoadCharacter(const int32_t imageSize)
    {
        const int32_t id = GetValueOfVariable("id");
        if (id == SPACE_ASCII)
        {
            mSpaceWidth = (GetValueOfVariable("xadvance") - mPaddingWidth) * mHorizontalPerPixelSize;
            return std::nullopt;
        }
        const float xTex = ((float)GetValueOfVariable("x") + (mPadding[PAD_LEFT] - DESIRED_PADDING)) / imageSize;
        const float yTex = ((float)GetValueOfVariable("y") + (mPadding[PAD_TOP] - DESIRED_PADDING)) / imageSize;
        const int32_t width = GetValueOfVariable("width") - (mPaddingWidth - (2 * DESIRED_PADDING));
        const int32_t height = GetValueOfVariable("height") - ((mPaddingHeight) - (2 * DESIRED_PADDING));
        const float quadWidth = width * mHorizontalPerPixelSize;
        const float quadHeight = height * mVerticalPerPixelSize;
        const float xTexSize = (float)width / imageSize;
        const float yTexSize = (float)height / imageSize;
        const float xOff = (GetValueOfVariable("xoffset") + mPadding[PAD_LEFT] - DESIRED_PADDING) * mHorizontalPerPixelSize;
        const float yOff = (GetValueOfVariable("yoffset") + (mPadding[PAD_TOP] - DESIRED_PADDING)) * mVerticalPerPixelSize;
        const float xAdvance = (GetValueOfVariable("xadvance") - mPaddingWidth) * mHorizontalPerPixelSize;
        return Character(id, xTex, yTex, xTexSize, yTexSize, xOff, yOff, quadWidth, quadHeight, xAdvance);
    }
}