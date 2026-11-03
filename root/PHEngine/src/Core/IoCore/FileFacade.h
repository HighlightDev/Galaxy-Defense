#pragma once

#include <filesystem>
#include <functional>
#include <list>
#include <string>

class FileFacade {

    std::list<std::string> mFileSrc;

    std::filesystem::path mPathToFile;

public:
    FileFacade() = default;

    bool OpenAndReadFile(const std::filesystem::path& pathToFile);

    size_t GetFileSourceLinesCount() const;

    void AppendToTheSrcEnd(const std::string& line);
    void AppendAtTheSrcBiginning(const std::string& line);
    void InsertInSrc(const std::string& line, const size_t index);
    void RewriteSrc(const std::string& newSrc);
    void WriteToFile() const;

    const std::list<std::string>& GetFileSrc() const;
    std::string SeparateByFunctor(
        const size_t index,
        std::function<std::string(const std::string&, const std::string&)> separateFunctor,
        const std::string& separateBy) const;
    bool ReplaceSourceLineAt(const size_t index, const std::string& insertText);

    bool CheckIfFileExists(const std::filesystem::path& pathToFile);

private:
    bool LoadFile(const std::filesystem::path& pathToFile);

    std::string ConcatFileSrc() const;
};
