#include "GameSkeletalMeshComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GameCore/ScriptingCore/LuaWrapper.h"
#include "Core/GameCore/ScriptingCore/LuaCore.inl"

namespace Labyrinth
{
   GameSkeletalMeshComponent::GameSkeletalMeshComponent(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale, const std::string& mLuaScriptRelPath,
      const SkeletalMeshRenderData& renderData)
      : SkeletalMeshComponent(translation, rotation, scale, mLuaScriptRelPath, renderData)
      , mLuaInstance(std::make_unique<LuaWrapper>())
   {
   }

   GameSkeletalMeshComponent::~GameSkeletalMeshComponent()
   {
   }

   void GameSkeletalMeshComponent::Tick(const float deltaTime)
   {
      if (mLuaInstance->ExecuteScript(mLuaScriptRelPath))
      {
         const float updatedTime = LuaFunction<float(float)>::Call(*mLuaInstance.get(), "UpdateAnimationTime", deltaTime);
         mSrcAnimationTime += updatedTime;
      }

      mUpdateDataResetTimeCounter += deltaTime;
      const bool bUpdateData = mUpdateDataResetTimeCounter >= update_data_reset_time;

      if (bUpdateData)
      {
         SyncDataWithRenderThread();
      }
   }
  
}