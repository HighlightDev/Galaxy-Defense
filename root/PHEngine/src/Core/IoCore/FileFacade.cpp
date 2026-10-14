#include "FileFacade.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <filesystem>
#include <fstream>
#include <iterator>

bool FileFacade::OpenAndReadFile(const std::string& pathToFile)
{
    ext_assert(pathToFile != "", "FileFacade::OpenAndReadFile: pathToFile is empty");
    mPathToFile = pathToFile;
    return LoadFile(pathToFile);
}

const std::list<std::string>& FileFacade::GetFileSrc() const
{
    return mFileSrc;
}

bool FileFacade::LoadFile(const std::string& pathToFile)
{
    if (CheckIfFileExists(pathToFile)) {
        std::ifstream stream(pathToFile);
        std::string line;
        while (stream.is_open() && getline(stream, line)) {
            mFileSrc.emplace_back(std::move(line));
        }

        return mFileSrc.size() > 0;
    }
    return false;
}

bool FileFacade::CheckIfFileExists(const std::string& pathToFile)
{
    return std::filesystem::exists(pathToFile);
}

size_t FileFacade::GetFileSourceLinesCount() const
{
    return mFileSrc.size();
}

void FileFacade::AppendToTheSrcEnd(const std::string& line)
{
    mFileSrc.emplace_back(std::move(line));
}

void FileFacade::AppendAtTheSrcBiginning(const std::string& line)
{
    mFileSrc.emplace_front(std::move(line));
}

void FileFacade::InsertInSrc(const std::string& line, const size_t index)
{
    auto it = std::next(mFileSrc.begin(), index);
    mFileSrc.emplace(it, std::move(line));
}

bool FileFacade::ReplaceSourceLineAt(const size_t index, const std::string& insertText)
{
    bool bResult = false;

    if (index >= 0 && index < mFileSrc.size()) {
        (*std::next(mFileSrc.begin(), index)) = insertText;
        bResult = true;
    }

    return bResult;
}

void FileFacade::RewriteSrc(const std::string& newSrc)
{
    mFileSrc.clear();
    mFileSrc.emplace_back(newSrc);
}

std::string FileFacade::SeparateByFunctor(
    const size_t index,
    std::function<std::string(const std::string&, const std::string&)> separateFunctor,
    const std::string& separateBy) const
{
    std::string result;

    if (index >= 0 && index < mFileSrc.size()) {
        auto it = std::next(mFileSrc.begin(), index);
        const std::string& lookupString = *(it);

        result = separateFunctor(lookupString, separateBy);
    }

    return result;
}

void FileFacade::WriteToFile() const
{
    ext_assert(mPathToFile != "", "FileFacade::WriteToFile: mPathToFile is empty");
    std::ofstream stream;
    stream.open(mPathToFile, std::fstream::out);

    const std::string& result = ConcatFileSrc();
    stream << result;
}

std::string FileFacade::ConcatFileSrc() const
{
    std::string result;

    auto lastElementIt = std::next(mFileSrc.end(), -1);

    for (std::list<std::string>::const_iterator it = mFileSrc.begin(); it != mFileSrc.end(); ++it) {
        std::string str = *it;
        str = EngineUtility::TrimEnd(str);

        if (lastElementIt != it)
            result += str + "\n";
        else
            result += str;
    }

    return result;
}
