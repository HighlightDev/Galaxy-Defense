#include "LuaUiControllerExecutor.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/CommonCore/StringHash.h"

using namespace EngineMath;
using namespace EngineCore;

namespace Game
{
   LuaUiControllerExecutor::LuaUiControllerExecutor(const std::string &scriptName)
       : LuaCommonEngineFunctions(scriptName)
   {
   }

   LuaUiControllerExecutor::~LuaUiControllerExecutor()
   {
   }

   void LuaUiControllerExecutor::RegisterCallbacks()
   {
      LuaCommonEngineFunctions::RegisterCallbacks();
      //LuaCallbackBindingHelper<Hash64_CT("LuaUiControllerExecutor::AddRoutePoint"), void(std::string, glm::vec3, glm::vec3, glm::vec3, float)>::Bind(mLuaInstance, this, std::bind(&LuaUiControllerExecutor::AddRoutePoint, this, std::placeholders::_1), "_AddRoutePoint");
   }
}