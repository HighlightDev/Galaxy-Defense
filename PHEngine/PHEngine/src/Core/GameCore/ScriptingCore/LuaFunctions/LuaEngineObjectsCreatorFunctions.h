#pragma once
#include <unordered_map>
#include <glm/vec3.hpp>
#include <memory>

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreator.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ACamera.h"
#include "ILuaFunctionable.h"

using namespace EngineCore;

namespace EngineCore
{
   namespace Scripts
   {
      class LuaScriptExecutorBase;
      class LuaScriptProcessor;

      class LuaEngineObjectsCreatorFunctions
          : public ILuaFunctionable
      {
         LuaScriptExecutorBase *mOwnerPtr;

         std::weak_ptr<Scene> mSceneWp;

         std::weak_ptr<LuaScriptProcessor> mLuaScriptProcessor;

         std::shared_ptr<EngineObjectCreator> mEngineObjectCreator;

      public:
         LuaEngineObjectsCreatorFunctions(LuaScriptExecutorBase *ownerPtr);

         ~LuaEngineObjectsCreatorFunctions();

         void Initialize() override;

         void SetScene(const std::weak_ptr<Scene> &sceneWp) override;

         void SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor> &scriptProcessor) override;

         void OnScriptStarted(const LuaWrapper &luaWrapper) override;

         void OnScriptStopped(const LuaWrapper &luaWrapper) override;

         void RegisterCallbacks(const LuaWrapper &luaWrapper) override;

         std::shared_ptr<EngineObjectCreator> GetEngineObjectCreator() const;

      private:
         // Common callbacks
         /* -------------------  Load asynchronously resources by names ----------------------------*/
         void LazyLoadResourcesAsync(const std::tuple<std::string> &dataNames);

         void OpenAudioStreams(const std::tuple<std::string> &dataName);

         /* -------------------  Create Actor ----------------------------*/
         int32_t CreateActor(const std::tuple<std::string /*Actor type*/,
                                              std::string /*Actor name*/,
                                              glm::vec3 /*root component translation*/,
                                              glm::vec3 /*root component rotation*/,
                                              glm::vec3 /*root component scale*/,
                                              std::string /*json params string*/> &actorData);
         /* -------------------  Create component and attach to actor ----------------------------*/
         void CreateAndAttachComponentToActor(const std::tuple<int32_t /*actorObjectId*/,
                                                               std::string /*componentType*/,
                                                               std::string /*component data json*/> &componentData);

         /* -------------------  Create third person camera ----------------------------*/
         void CreateThirdPersonCamera(const std::tuple<std::string, glm::ivec4, std::string /*viewProjectionJsonArgs*/, float, float, float, glm::vec3, int32_t> &cameraData);

         /* -------------------  Create first person camera ----------------------------*/
         void CreateFirstPersonCamera(const std::tuple<std::string, glm::ivec4, std::string /*viewProjectionJsonArgs*/, float, float, glm::vec3, int32_t> &cameraData);

         /* -------------------  Create material instance and register --------------------*/
         int32_t CreateMaterial(const std::tuple<std::string> &buildMaterial);

         /* -------------------  Set texture --------------------*/
         void SetTextureToMaterial(const std::tuple<int32_t /*material proxy id*/, std::string /* texture name*/, std::string /*property name*/> &setTextureToMaterial);

         /* -------------------  Set float --------------------*/
         void SetFloatToMaterial(const std::tuple<int32_t /*material proxy id*/, float, std::string> &setFloatValueToMaterial);

         /* -------------------  Set deferred texture --------------------*/
         void SetDeferredTextureToMaterial(const std::tuple<int32_t /*material proxy id*/, /*deferred resource creator name*/ std::string, /*property name*/ std::string> &setDeferredTextureToMaterial);

         /* -------------------  Set binding to material --------------------*/
         void SetBindingToMaterial(const std::tuple<int32_t /*material proxy id*/, /*game object name*/ std::string, /*gamePropertyName*/ std::string, /*bindingName*/ std::string> &setBindingToMaterial);

         /* -------------------  Create Tweener ----------------------------*/
         int32_t CreateTweener(const std::tuple<int32_t /*Actor id*/, std::string /*tweener name*/> &tweenerData);

         /* -------------------  Set tweener bindings ------------------------*/
         void SetTweenerBinding(const std::tuple<int32_t /*Actor id which holds tweener*/,
                                                 int32_t /*tweener id*/,
                                                 std::string /*game object name*/,
                                                 std::string /*binding name*/,
                                                 std::string /*property name*/> &tweenerData);
      };
   }
}
