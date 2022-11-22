#include "LuaPlatformTraverseComponentFunctions.h"
#include "Core/GameCore/Components/PlatformTraverseComponent.h"
#include "Core/UtilityCore/EngineMath.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/CommonCore/StringHash.h"

using namespace EngineMath;
using namespace EngineCore;

namespace EngineCore
{
   namespace Scripts
   {
      LuaPlatformTraverseComponentFunctions::LuaPlatformTraverseComponentFunctions(PlatformTraverseComponent *owner, const std::string &scriptName)
          : LuaCommonEngineFunctions(scriptName),
            mOwnerComponent(owner)
      {
      }

      LuaPlatformTraverseComponentFunctions::~LuaPlatformTraverseComponentFunctions()
      {
      }

      void LuaPlatformTraverseComponentFunctions::RegisterCallbacks()
      {
         LuaCallbackBindingHelper<Hash64_CT("LuaPlatformTraverseComponentFunctions::AddRoutePoint"), void(std::string, glm::vec3, glm::vec3, glm::vec3, float)>::Bind(mLuaInstance, this, std::bind(&LuaPlatformTraverseComponentFunctions::AddRoutePoint, this, std::placeholders::_1), "_AddRoutePoint");
      }

      void LuaPlatformTraverseComponentFunctions::RunScript()
      {
         LuaCommonEngineFunctions::RunScript();

         mOwnerComponent->SetDestinationPoint(GetLuaGlobalVariable<std::string>::Value(mLuaInstance, "StartRoute", -1));
      }

      void LuaPlatformTraverseComponentFunctions::AddRoutePoint(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, float> &data)
      {
         const std::string &name = std::get<0>(data);
         const glm::vec3 &translation = std::get<1>(data);

         const glm::vec3 &eulerRotation = std::get<2>(data);
         const glm::vec3 &scaling = std::get<3>(data);
         const float transitionTime = std::get<4>(data);

         mOwnerComponent->AddMovementPoint(name, EulerAnglesTransform(translation, eulerRotation, scaling), transitionTime);
      }
   }
}