#include "LuaScriptExecutor_MovementComponent.h"
#include "Core/GameCore/Components/PlatformMovementComponent.h"
#include "Core/UtilityCore/EngineMath.h"

using namespace EngineMath;

namespace Game
{
  
   LuaScriptExecutor_MovementComponent::LuaScriptExecutor_MovementComponent(PlatformMovementComponent* owner, const std::string& scriptName)
      : LuaScriptExecutor_EngineBase(scriptName)
      , mOwnerComponent(owner)
   {

   }

   LuaScriptExecutor_MovementComponent::~LuaScriptExecutor_MovementComponent()
   {

   }

   // Add route point
   void LuaScriptExecutor_MovementComponent::RegisterCallbacks()
   {
      LuaScriptExecutor_EngineBase::RegisterCallbacks();

      using LuaExecutor_t = LuaScriptExecutor_MovementComponent;

      LuaRegisterCallback<LuaExecutor_t, void(std::string, glm::vec3, glm::vec3, glm::vec3, float)>::Register(mLuaInstance, "_AddRoutePoint");
   }

   void LuaScriptExecutor_MovementComponent::RunScript()
   {
      LuaScriptExecutor_EngineBase::RunScript();

      mOwnerComponent->SetDestinationPoint(LuaGetGlobal<std::string>::Value(mLuaInstance, "StartRoute", -1));
   }

   void LuaScriptExecutor_MovementComponent::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::vec3, glm::vec3, float>& data)
   {
      const std::string& name = std::get<0>(data);
      const glm::vec3& translation = std::get<1>(data);
   
      const glm::vec3& eulerRotation = std::get<2>(data);
      const glm::vec3& scaling = std::get<3>(data);
      const float transitionTime = std::get<4>(data);

      mOwnerComponent->AddMovementPoint(name, EulerAnglesTransform(translation, eulerRotation, scaling), transitionTime);
   }
}