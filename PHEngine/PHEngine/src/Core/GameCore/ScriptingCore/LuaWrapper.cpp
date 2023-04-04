#include "LuaWrapper.h"
#include "Core/UtilityCore/PlatformDependentFunctions.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/IoCore/FileFacade.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/CommonCore/Assertion.h"

#include <algorithm>

using namespace TinyLogger;
using namespace IO;

namespace EngineCore
{
   namespace Scripts
   {

      LuaWrapper::LuaWrapper()
      {
         mState = luaL_newstate();
         luaL_openlibs(mState);
      }

      LuaWrapper::~LuaWrapper()
      {
         StopExecution();
         mState = nullptr;
      }

      bool LuaWrapper::ExecuteScript(const std::string &absPath)
      {
#ifdef DEBUG
         CompileRequiredScripts(absPath);
#endif
         if (luaL_dofile(mState, absPath.c_str()) != LUA_OK)
         {
            const auto &message = GetErrorMessageAt(-1);
            LogInfo(message);
            return false;
         }

         mIsLuaScriptOpened = true;
         return true;
      }

      void LuaWrapper::StopExecution()
      {
         if (mIsLuaScriptOpened && mState)
         {
            lua_close(mState);
            mIsLuaScriptOpened = false;
         }
      }

      std::string LuaWrapper::GetErrorMessageAt(int32_t stackIndex) const
      {
         return lua_tostring(mState, stackIndex);
      }

      lua_State *LuaWrapper::GetState() const
      {
         return mState;
      }

#ifdef DEBUG
      void LuaWrapper::ProcessLuaCompilation(const std::string &pathToScript, std::unordered_set<std::string> &alreadyCompiledModules)
      {
         FileFacade scriptFile(pathToScript);
         const auto &scriptSrc = scriptFile.GetFileSrc();
         const auto &requiredModules = GetAllRequiredLuaModules(scriptSrc, alreadyCompiledModules);
         for (const auto &requiredModule : requiredModules)
         {
            if (!alreadyCompiledModules.count(requiredModule))
            {
               alreadyCompiledModules.insert(requiredModule);
               const auto &folderMngr = FolderManager::GetInstance();
               const auto relativePathToScript = folderMngr->GetRelativePathToFile(requiredModule + ".lua");
               assert("" != relativePathToScript);
               const auto absPathToScript = folderMngr->GetPathToExeFile() + relativePathToScript + requiredModule + ".lua";
               ProcessLuaCompilation(absPathToScript, alreadyCompiledModules);
               const auto errorMsg = CompileLuaModuleAndGetError(requiredModule + ".lua");
               if (errorMsg != "")
               {
                  LogInfo("ERROR: Lua script execution failed:", errorMsg);
               }
            }
         }
      }

      std::vector<std::string> LuaWrapper::GetAllRequiredLuaModules(const std::list<std::string> &srcList, const std::unordered_set<std::string> &compiledModules) const
      {
         std::vector<std::string> result;

         for (const auto &luaLine : srcList)
         {
            const std::string requireStr = "require";
            if (EngineUtility::HasSubstringPresence(luaLine, requireStr) && !EngineUtility::HasSubstringPresence(luaLine, "3rdparty"))
            {
               const auto startIndex = EngineUtility::IndexOf(luaLine, requireStr);
               if (startIndex != std::string::npos)
               {
                  const auto endIndex = EngineUtility::LastIndexOf(luaLine, "\")");
                  if (endIndex != std::string::npos)
                  {
                     const auto &requiredModulePath = EngineUtility::Trim(luaLine.substr(startIndex + requireStr.size(), endIndex - (startIndex + requireStr.size())));
                     std::string moduleName = "";
                     if (EngineUtility::HasSubstringPresence(requiredModulePath, "/"))
                     {
                        const auto splitedPathToModule = EngineUtility::Split(requiredModulePath, '/');
                        moduleName = splitedPathToModule.back();
                     }
                     else if (EngineUtility::HasSubstringPresence(requiredModulePath, "\""))
                     {
                        const auto splitedPathToModule = EngineUtility::Split(requiredModulePath, '\"');
                        moduleName = splitedPathToModule.back();
                     }
                     else 
                     {
                        assert(false);
                     }

                     if (!compiledModules.count(moduleName))
                     {
                        result.emplace_back(moduleName);
                     }
                  }
               }
            }
         }

         return result;
      }

      std::string LuaWrapper::CompileLuaModuleAndGetError(const std::string &luaModuleName) const
      {
         const auto &folderMngr = FolderManager::GetInstance();
         const auto relativePathToScript = folderMngr->GetRelativePathToFile(luaModuleName);
         assert("" != relativePathToScript);
         const auto absPathToScript = folderMngr->GetPathToExeFile() + relativePathToScript + luaModuleName;

         std::string compilationErrorMsg;
         lua_State *compileLuaState = luaL_newstate();
         luaL_openlibs(compileLuaState);
         if (luaL_dofile(compileLuaState, absPathToScript.c_str()) != LUA_OK)
         {
            compilationErrorMsg = lua_tostring(compileLuaState, -1);
         }
         lua_close(compileLuaState);

         return compilationErrorMsg;
      }

      void LuaWrapper::CompileRequiredScripts(const std::string &pathToEntryScript)
      {
         std::unordered_set<std::string> compiledScripts;
         ProcessLuaCompilation(pathToEntryScript, compiledScripts);
      }
#endif
   }
}