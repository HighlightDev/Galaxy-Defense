#include "LuaScriptExecutor_MovementComponent.h"
#include "Core/GameCore/Components/MovementComponent.h"

namespace Game
{
  
   LuaScriptExecutor_MovementComponent::LuaScriptExecutor_MovementComponent(MovementComponent* owner, const std::string& scriptName)
      : mOwnerComponent(owner)
      , mScriptName(scriptName)
   {

   }

   LuaScriptExecutor_MovementComponent::~LuaScriptExecutor_MovementComponent()
   {

   }

   // Add route point
   void LuaScriptExecutor_MovementComponent::RegisterCallbacks()
   {
      using LuaExecutor_t = LuaScriptExecutor_MovementComponent;

      LuaRegisterCallback<LuaExecutor_t, void(std::string, glm::vec3, glm::quat, glm::vec3)>::Register(mLuaInstance, "_AddRoutePoint");
   }

   void LuaScriptExecutor_MovementComponent::RunScript()
   {
      const auto& folderManager = FolderManager::GetInstance();
      const bool bScriptExecuted = mLuaInstance.ExecuteScript(EngineUtility::ConvertFromRelativeToAbsolutePath(folderManager->GetScriptPath() + mScriptName));

      assert(bScriptExecuted);

      LuaFunction<void(void*)>::Call(mLuaInstance, "InitRoutes", (void*)this);

      mOwnerComponent->SetDestinationPoint(LuaGetGlobal<std::string>::Value(mLuaInstance, "StartRoute", -1));
   }

   void LuaScriptExecutor_MovementComponent::ExecuteLuaCallback(const std::tuple<std::string, glm::vec3, glm::quat, glm::vec3>& data)
   {
      const std::string& name = std::get<0>(data);
      const glm::vec3& translation = std::get<1>(data);
      const glm::quat& rotator = std::get<2>(data);
      const glm::vec3& scaling = std::get<3>(data);

      mOwnerComponent->AddMovementPoint(name, Transform(translation, rotator, scaling));
   }
}