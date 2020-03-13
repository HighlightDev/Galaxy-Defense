#define _CRT_SECURE_NO_WARNINGS 

#include "IShader.h"
#include "Core/CommonCore/FolderManager.h"
#include "Core/IoCore/FileFacade.h"

#include <fstream>
#include <algorithm>
#include <set>

#include <filesystem>
#include <chrono>

namespace Graphics
{
   namespace OpenGL
   {
      IShader::IShader(const std::string& shaderName)
         : m_vertexShaderID(-1)
         , m_fragmentShaderID(-1)
         , m_geometryShaderID(-1)
         , m_shaderProgramID(-1)
         , mShaderName(shaderName)
      {
      }
   
      IShader::~IShader()
      {
      }

      Uniform IShader::GetUniform(const std::string& uniformName, uint32_t shaderProgramID) const
      {
         try
         {
            return Uniform(shaderProgramID, uniformName);
         }
         catch (std::invalid_argument innerEx)
         {
            std::string message;
            EngineUtility::StringStreamWrapper::ToString("Shader with name", mShaderName, " could not bind uniform(s);", "Inner exception message :", '\n', innerEx.what());

            throw std::invalid_argument(EngineUtility::StringStreamWrapper::FlushString());
         }
      }

      UniformArray IShader::GetUniformArray(const std::string& uniformName, size_t countOfUniforms, uint32_t shaderProgramID) const
      {
         try
         {
            return UniformArray(shaderProgramID, countOfUniforms, uniformName);
         }
         catch (std::invalid_argument innerEx)
         {
            std::string message;
            EngineUtility::StringStreamWrapper::ToString("Shader with name", mShaderName, " could not bind uniform(s);", "Inner exception message :", '\n', innerEx.what());

            throw std::invalid_argument(EngineUtility::StringStreamWrapper::FlushString());
         }
      }

      template <typename TP>
      std::time_t to_time_t(TP tp)
      {
         using namespace std::chrono;
         auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
            + system_clock::now());
         return system_clock::to_time_t(sctp);
      }

      bool IShader::IsShaderSourceFileChanged(const std::string& shaderPath) const
      {
         bool bResult = false;

         std::string pathToPersistency = EngineUtility::ConvertFromRelativeToAbsolutePath(
            Common::FolderManager::GetInstance()->GetPersistencyPath() + "shader_persistency");

         std::string pathToShader = EngineUtility::ConvertFromRelativeToAbsolutePath(shaderPath);

         std::filesystem::file_time_type fTimeType = std::filesystem::last_write_time(pathToShader);
         std::time_t cftime = to_time_t(fTimeType);

#undef _CRT_SECURE_NO_WARNINGS

         std::string timeFileWasChanged = EngineUtility::TrimEnd(std::asctime(std::localtime(&cftime)));

         FileFacade fileFacade = FileFacade(pathToPersistency);

         const auto& fileSrcList = fileFacade.GetFileSrc();

         bool bShaderExistsInPersistency = false;

         size_t index = 0;
         for (auto it = fileSrcList.begin(); it != fileSrcList.end(); ++it, ++index)
         {
            if (EngineUtility::StartsWith(*it, shaderPath))
            {
               bShaderExistsInPersistency = true;
               break;
            }
         }

         if (!bShaderExistsInPersistency)
         {
            // if this shader doesn't exist in persistency 
            // then write it to persistency and mark that shader was changed
            std::string resultString = shaderPath + " " + timeFileWasChanged;
            fileFacade.AppendToTheSrcEnd(resultString);
            fileFacade.WriteToFile();
            bResult = true;
         }
         else
         {
            auto separateTimeFunctor = std::function<std::string(const std::string&, const std::string&)>(([](const std::string& lookupString, const std::string& separateBy)
            {
               return lookupString.substr(EngineUtility::IndexOf(lookupString, separateBy) + 1);
            }));

            std::string timeFromPersistancy = fileFacade.SeparateByFunctor(index, separateTimeFunctor, " ");

            if (timeFromPersistancy != timeFileWasChanged)
            {
               std::string replaceShaderPersistance = shaderPath + " " + timeFileWasChanged;
               fileFacade.ReplaceSourceLineAt(index, replaceShaderPersistance);
               fileFacade.WriteToFile();
               bResult = true;
            }
         }

         return bResult;
      }

      std::vector<std::string> IShader::LoadShaderSrcVector(const std::string& pathToShader) const
      {
         std::ifstream stream(pathToShader);
         std::string line;

         std::vector<std::string> code;

         while (stream.is_open() && getline(stream, line))
         {
            code.emplace_back(line);
         }

         stream.clear();
         stream.close();

         return code;
      }

      bool IShader::ProcessShaderIncludes(std::string& shaderSource)
      {
         bool bProcessInclude = false;

         if ("" != shaderSource)
         {
            std::vector<std::string> sourceVector = EngineUtility::Split(shaderSource, '\n');

            std::string includingSources = "", versionInfo = "";
            std::set<std::string> includes;

            for (auto it = sourceVector.begin(); it != sourceVector.end();)
            {
               if (EngineUtility::StartsWith(*it, "#version"))
               {
                  versionInfo = *it + "\n";
                  it = sourceVector.erase(it);
               }
               else if (EngineUtility::StartsWith(*it, "#include"))
               {
                  size_t indexName = EngineUtility::IndexOf(*it, " ");
                  if (std::string::npos != indexName)
                  {
                     std::string name = it->substr(indexName + 1);

                     if (includes.end() == includes.find(name)) // New include
                     {
                        includes.insert(name);
                        name.erase(std::remove(name.begin(), name.end(), '\"'), name.end()); // remove quotes

                        const std::string absolutePath = EngineUtility::ConvertFromRelativeToAbsolutePath(Common::FolderManager::GetInstance()->GetShaderCommonPath() + name);
                        includingSources += LoadShaderSource(absolutePath) + "\n";
                     }
                     it = sourceVector.erase(it);
                  }
               }
               else
               {
                  it++;
               }
            }

            for (auto item : sourceVector)
            {
               EngineUtility::StringStreamWrapper::ToString(item, "\n");
            }

            shaderSource = versionInfo + includingSources + EngineUtility::StringStreamWrapper::FlushString();
            bProcessInclude = true;
         }

         return bProcessInclude;
      }

      bool IShader::SendToGpuSingleShaderSource(int32_t shaderId, const std::string& shaderSource) const
      {
         bool bLoadResult = false;

         try
         {
            const char *c_str = shaderSource.c_str();
            glShaderSource(shaderId, 1, &c_str, nullptr);

            bLoadResult = true;
         }
         catch (...)
         {

         }

         return bLoadResult;
      }

      std::string IShader::LoadShaderSource(const std::string& pathToShader) const
      {
         std::string result = "";

         if ("" != pathToShader)
         {
            std::ifstream stream(pathToShader);
            std::string line;

            while (stream.is_open() && getline(stream, line))
            {
               result += line + "\n";
            }

            stream.clear();
            stream.close();
         }

         return result;
      }

      void IShader::SetIsShaderSourceFileChanged(const std::string& vsPath, const std::string& gsPath, const std::string& fsPath)
      {
         if ("" != vsPath)
            bUpdateVS = IsShaderSourceFileChanged(vsPath);
         if ("" != gsPath)
            bUpdateGS = IsShaderSourceFileChanged(gsPath);
         if ("" != fsPath)
            bUpdateFS = IsShaderSourceFileChanged(fsPath);
      }

      bool IShader::SendToGpuShadersSources(std::string& vsSource, std::string& gsSource, std::string& fsSource)
      {
         bool bVertexShaderLoaded = true, bFragmentShaderLoaded = true, bGeometryShaderLoaded = true;

         if (vsSource != "")
         {
            /*Vertex shader load*/
            m_vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
            bVertexShaderLoaded &= SendToGpuSingleShaderSource(m_vertexShaderID, vsSource);
         }

         if (fsSource != "")
         {
            /*Fragment shader load*/
            m_fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
            bFragmentShaderLoaded &= SendToGpuSingleShaderSource(m_fragmentShaderID, fsSource);
         }

         if (gsSource != "")
         {
            /*Geometry shader load*/
            m_geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
            bGeometryShaderLoaded &= SendToGpuSingleShaderSource(m_geometryShaderID, gsSource);
         }

         return bVertexShaderLoaded && bFragmentShaderLoaded && bGeometryShaderLoaded;
      }

      void IShader::WriteShaderSrc(const std::string& pathToShader, const std::string& src) const
      {
         std::ofstream writeStream(pathToShader);
         writeStream << src;
      }

      std::string IShader::GetPredefinedSource(std::vector<std::string>& shaderSourceVector, const std::vector<ShaderGenericDefineConstant>& constantDefines, const std::vector<ShaderGenericDefine>& defines) const
      {

         // src only with macros
         std::vector<ShaderGenericDefineConstant> existingConstantDefines;
         std::vector<ShaderGenericDefine> existingDefines;

         for (auto it = shaderSourceVector.begin(); it != shaderSourceVector.end();)
         {
            if (EngineUtility::StartsWith(*it, "#define"))
            {
               size_t indexName = EngineUtility::IndexOf(*it, " ");
               size_t indexValue = EngineUtility::IndexOf(*it, " ", indexName + 1);

               const std::string name = it->substr(indexName + 1, indexValue - indexName - 1);
              
               if (std::string::npos == indexValue)
               {
                  existingDefines.emplace_back(name, true);
               }
               else
               {
                  std::string value = it->substr(indexValue + 1);
                  existingConstantDefines.emplace_back(name, value);
               }

               // remove all macros from code
               it = shaderSourceVector.erase(it);
            }
            else
               ++it;
         }

         // update values for existing macros
         for (auto new_it = constantDefines.begin(); new_it != constantDefines.end(); ++new_it)
         {
            for (auto existing_it = existingConstantDefines.begin(); existing_it != existingConstantDefines.end();)
            {
               if (existing_it->m_Name == new_it->m_Name)
               {
                  existing_it = existingConstantDefines.erase(existing_it);
               }
               else
               {
                  ++existing_it;
               }
            }

            existingConstantDefines.push_back(*new_it);
         }

         // update defines
         for (auto new_it = defines.begin(); new_it != defines.end(); ++new_it)
         {
            for (auto existing_it = existingDefines.begin(); existing_it != existingDefines.end();)
            {
               if (existing_it->m_Name == new_it->m_Name)
               {
                  existing_it = existingDefines.erase(existing_it);
               }
               else
               {
                  ++existing_it;
               }
            }

            if (new_it->bDefined)
            {
               existingDefines.push_back(std::move(*new_it));
            }
         }

         for (auto it = existingConstantDefines.begin(); it != existingConstantDefines.end(); ++it)
         {
            EngineUtility::StringStreamWrapper::ToString("#define ", it->m_Name, " ", it->m_Value, '\n');
         }
         const std::string constantDefinesResult = std::move(EngineUtility::StringStreamWrapper::FlushString());

         for (auto it = existingDefines.begin(); it != existingDefines.end(); ++it)
         {
            EngineUtility::StringStreamWrapper::ToString("#define ", it->m_Name, '\n');
         }
         const std::string definesResult = std::move(EngineUtility::StringStreamWrapper::FlushString());

         std::vector<std::string>::iterator version_it = shaderSourceVector.begin();

         for (auto it = shaderSourceVector.begin(); it != shaderSourceVector.end(); ++it, ++version_it)
         {
            if (EngineUtility::StartsWith(*it, "#version"))
            {
               version_it += 2;
               break;
            }
         }

         if ("" != constantDefinesResult)
         {
            shaderSourceVector.insert(version_it, constantDefinesResult);
         }

         version_it = shaderSourceVector.begin();
         for (auto it = shaderSourceVector.begin(); it != shaderSourceVector.end(); ++it, ++version_it)
         {
            if (EngineUtility::StartsWith(*it, "#version"))
            {
               version_it += 2;
               break;
            }
         }

         if ("" != definesResult)
         {
            shaderSourceVector.insert(version_it, definesResult);
         }

         std::string codeResult = "";

         for (std::vector<std::string>::iterator it = shaderSourceVector.begin(); it != shaderSourceVector.end(); ++it)
         {
            std::string& str = *it;
            str = EngineUtility::TrimEnd(str);

            if (shaderSourceVector.end() - 1 != it)
               codeResult += str + "\n";
            else
               codeResult += str;
         }

         return codeResult;
      }

      void IShader::ProcessPredefineToSource(std::string& shaderSource, const std::vector<ShaderGenericDefineConstant>& constantDefines, const std::vector<ShaderGenericDefine>& defines) const
      {
         auto shaderSrc = EngineUtility::Split(shaderSource, '\n');

         shaderSource = GetPredefinedSource(shaderSrc, constantDefines, defines);
      }

      void IShader::ProcessPredefineToFile(const std::string& pathToShader, const std::vector<ShaderGenericDefineConstant>& constantDefines, const std::vector<ShaderGenericDefine>& defines) const
      {
         if (pathToShader == "")
            return;

         auto shaderSrc = LoadShaderSrcVector(pathToShader);

         std::string result = GetPredefinedSource(shaderSrc, constantDefines, defines);

         WriteShaderSrc(pathToShader, result);
      }

      void IShader::CompileShaders()
      {
         glCompileShader(m_vertexShaderID);
         glCompileShader(m_fragmentShaderID);
         glCompileShader(m_geometryShaderID);
      }


      void IShader::LinkShaders() const
      {
         glAttachShader(m_shaderProgramID, m_vertexShaderID);
         glAttachShader(m_shaderProgramID, m_fragmentShaderID);
         if (m_geometryShaderID != -1)
         {
            glAttachShader(m_shaderProgramID, m_geometryShaderID);
         }
         glLinkProgram(m_shaderProgramID);

         glDetachShader(m_shaderProgramID, m_vertexShaderID);
         glDetachShader(m_shaderProgramID, m_fragmentShaderID);
         if (m_geometryShaderID != -1)
         {
            glDetachShader(m_shaderProgramID, m_geometryShaderID);
         }
      }

      std::string IShader::GetCompileLogInfo() const
      {
         std::string compileLog;

         EngineUtility::StringStreamWrapper::FlushString(); // Just to clear stream
         GLint vertex_compiled = 0;
         if (m_vertexShaderID != -1)
         {
            glGetShaderiv(m_vertexShaderID, GL_COMPILE_STATUS, &vertex_compiled);
            if (vertex_compiled != GL_TRUE)
            {
               GLint log_length = 0;
               glGetShaderiv(m_vertexShaderID, GL_INFO_LOG_LENGTH, &log_length);

               std::vector<char> message(log_length);
               glGetShaderInfoLog(m_vertexShaderID, log_length, nullptr, message.data());
               EngineUtility::StringStreamWrapper::ToString("Vertex shader : ", std::string(message.begin(), message.end()), '\n');
            }
         }

         /*Fragment shader log info*/
         GLint fragment_compiled = 0;
         if (m_fragmentShaderID != -1)
         {
            glGetShaderiv(m_fragmentShaderID, GL_COMPILE_STATUS, &fragment_compiled);
            if (fragment_compiled != GL_TRUE)
            {
               GLint log_length = 0;
               glGetShaderiv(m_fragmentShaderID, GL_INFO_LOG_LENGTH, &log_length);

               std::vector<char> message(log_length);
               glGetShaderInfoLog(m_fragmentShaderID, log_length, nullptr, message.data());
               EngineUtility::StringStreamWrapper::ToString("Fragment shader : ", std::string(message.begin(), message.end()), '\n');
            }
         }


         GLint geometry_compiled = 0;
         /*Geometry shader log info*/
         if (m_geometryShaderID != -1)
         {
            glGetShaderiv(m_geometryShaderID, GL_COMPILE_STATUS, &geometry_compiled);
            if (geometry_compiled != GL_TRUE)
            {
               GLint log_length = 0;
               glGetShaderiv(m_geometryShaderID, GL_INFO_LOG_LENGTH, &log_length);

               std::vector<char> message(log_length);
               glGetShaderInfoLog(m_geometryShaderID, log_length, nullptr, message.data());
               EngineUtility::StringStreamWrapper::ToString("Geometry shader : ", std::string(message.begin(), message.end()), '\n');
            }
         }

         if ((m_vertexShaderID != -1 && vertex_compiled != GL_TRUE) || (m_fragmentShaderID != -1 && fragment_compiled != GL_TRUE) || (m_geometryShaderID != -1 && geometry_compiled != GL_TRUE))
            compileLog += std::string("Unresolved mistakes at :" + mShaderName + '\n') + EngineUtility::StringStreamWrapper::FlushString() + "\n";

         return compileLog;
      }

      std::string IShader::GetLinkLogInfo() const
      {
         std::string  linkLog;
         EngineUtility::StringStreamWrapper::FlushString(); // Just to clear stream
         GLint isLinked = 0;
         glGetProgramiv(m_shaderProgramID, GL_LINK_STATUS, &isLinked);

         if (isLinked == GL_FALSE)
         {
            GLint log_length = 0;
            glGetProgramiv(m_shaderProgramID, GL_INFO_LOG_LENGTH, &log_length);

            std::vector<char> message(log_length);
            glGetProgramInfoLog(m_shaderProgramID, log_length, nullptr, message.data());
            EngineUtility::StringStreamWrapper::ToString("Unsolved mistakes at :", mShaderName, "\n");
         }

         linkLog = std::move(EngineUtility::StringStreamWrapper::FlushString());
         linkLog = linkLog != "" ? linkLog + "\n" : linkLog;

         return linkLog;
      }

      bool IShader::IsShaderCompiled() const
      {
         return GetCompileLogInfo().size() <= 0;
      }

      void IShader::ExecuteShader() const
      {
         glUseProgram(m_shaderProgramID);
      }

      void IShader::StopShader() const
      {
         glUseProgram(0);
      }

      void IShader::AccessAllUniformLocations(uint32_t shaderProgramId)
      {
         m_shaderProgramID = shaderProgramId;
#if DEBUG
         std::cout << GetCompileLogInfo();
         std::cout << GetLinkLogInfo();
#endif
      }

      void IShader::CleanUp(bool bDeleteShaderProgram)
      {
         StopShader();
         glDetachShader(m_shaderProgramID, m_vertexShaderID);
         glDetachShader(m_shaderProgramID, m_fragmentShaderID);
         if (m_geometryShaderID != -1)
         {
            glDetachShader(m_shaderProgramID, m_geometryShaderID);
            glDeleteShader(m_geometryShaderID);
         }

         glDeleteShader(m_vertexShaderID);
         glDeleteShader(m_fragmentShaderID);

         if (bDeleteShaderProgram)
         {
            glDeleteShader(m_shaderProgramID);
         }
      }

   }
}