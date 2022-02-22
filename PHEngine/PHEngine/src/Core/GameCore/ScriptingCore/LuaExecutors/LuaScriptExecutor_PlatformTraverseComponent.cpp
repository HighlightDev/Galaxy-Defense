#include "LuaScriptExecutor_PlatformTraverseComponent.h"
#include "Core/GameCore/Components/PlatformTraverseComponent.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace EngineCore
{
  
   LuaScriptExecutor_PlatformTraverseComponent::LuaScriptExecutor_PlatformTraverseComponent(PlatformTraverseComponent* owner, const std::string& scriptName)
      : LuaScriptExecutor_EngineBase(scriptName)
      , mOwnerComponent(owner)
   {

   }

   LuaScriptExecutor_PlatformTraverseComponent::~LuaScriptExecutor_PlatformTraverseComponent()
   {

   }

   // Add route point
   void LuaScriptExecutor_PlatformTraverseComponent::RegisterCallbacks()
   {
      LuaScriptExecutor_EngineBase::RegisterCallbacks();

      using LuaExecutor_t = LuaScriptExecutor_PlatformTraverseComponent;

      LuaRegisterCallback<LuaExecutor_t, void(std::string, glm::vec3, glm::vec3, glm::vec3, float)>::Register(mLuaInstance, "_AddRoutePoint");
   }

   void LuaScriptExecutor_PlatformTraverseComponent::RunScript()
   {
      LuaScriptExecutor_EngineBase::RunScript();

      mOwnerComponent->SetDestinationPoint(LuaGetGlobal<std::string>::Value(mLuaInstance, "StartRoute", -1));
   }

   void LuaScriptExecutor_PlatformTraverseComponent::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, float>& data)
   {
      const std::string& name = std::get<0>(data);
      const glm::vec3& translation = std::get<1>(data);
   
      const glm::vec3& eulerRotation = std::get<2>(data);
      const glm::vec3& scaling = std::get<3>(data);
      const float transitionTime = std::get<4>(data);

      mOwnerComponent->AddMovementPoint(name, EulerAnglesTransform(translation, eulerRotation, scaling), transitionTime);
   }
}