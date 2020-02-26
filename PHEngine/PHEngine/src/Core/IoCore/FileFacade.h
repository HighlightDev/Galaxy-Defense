#pragma once

#include <fstream>
#include <list>
#include <string>
#include <functional>

class FileFacade
{

   std::list<std::string> mFileSrc;

   std::string mPathToFile;

public:

   FileFacade(const std::string& pathToFile);

   size_t GetFileSourceSize() const;

   void AppendToTheSrcEnd(const std::string& line);
   void AppendAtTheSrcBiginning(const std::string& line);
   void InsertInSrc(const std::string& line, const size_t index);
   void WriteToFile() const;

   const std::list<std::string> GetFileSrc() const;
   std::string SeparateByFunctor(const size_t index, std::function<std::string(const std::string&, const std::string&)> separateFunctor, const std::string& separateBy) const;
   bool ReplaceSourceLineAt(const size_t index, const std::string& insertText);

private:

   bool LoadFile(const std::string& pathToFile);

   std::string ConcatFileSrc() const;
};

