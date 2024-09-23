#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/ActorController.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/GameCore/Physics/DebugRender/DebugPhysicsRenderData.h"
#include "Core/GameCore/ACamera.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"
#include "Core/GraphicsCore/SceneViewInfo/CameraFrustum.h"
#include "TextHandler.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/DebugCore/DebugUiController.h"
#include "Core/GameCore/ScriptingCore/EngineToLuaReplicatorBase.h"
#include "Core/GameCore/Event/WindowSizeChangedEvent.h"
#include "Core/GameCore/Event/MouseButtonDownEvent.h"

#include <type_traits>
#include <glm/vec3.hpp>
#include <optional>
#include <unordered_map>

using namespace Thread;
using namespace Event;
using namespace Resources;
using namespace Graphics;
using namespace GUI;
using namespace EngineCore::Debug;
using namespace EngineCore::Scripts;

namespace Graphics
{
   class IMaterial;
   class DynamicMaterial;
   class MaterialProxy;
   class PlanarReflectionProxy;

   namespace Proxy
   {
      class LightSceneProxy;
      class PrimitiveSceneProxy;
      class UiCanvasSceneProxy;
      class UiSceneProxyBase;
   }
}

namespace EnginePhysics
{
   class PhysicsWorld;
}

namespace EngineCore
{
   class IComponentCreatable;
   struct ComponentData;
   class InstancedGeometryBatchHolder;

   class Scene : public EngineObject,
                 public ITickable,
                 public std::enable_shared_from_this<Scene>,
                 public WindowSizeChangedGameThreadEvent,
                 public MouseButtonDownRootEvent
   {
   private:
      std::shared_ptr<EnginePhysics::PhysicsWorld> mPhysicsWorld;

      std::vector<std::weak_ptr<EngineObject>> mEngineObjects;

      std::unordered_map<int32_t, std::shared_ptr<EngineToLuaReplicatorBase>> mLuaReplicators;

      bool mLuaReplicatorsDirty{false};

      InterThreadCommunicationMgr &m_interThreadMgr;

      std::shared_ptr<EngineObjectProperty<float>> mGameThreadDeltaSec;

      std::shared_ptr<EngineObjectProperty<glm::vec2>> mScreenResolutionProperty;

      std::unordered_map<std::string, std::weak_ptr<IDeferredResourceCreator>> mDeferredResourceCreators;

      std::vector<std::shared_ptr<Actor>> mActors;

      std::shared_ptr<ACamera> mMainCamera;

      std::vector<std::shared_ptr<ACamera>> mActiveCameras;

      std::vector<std::shared_ptr<ActorController>> mActorControllers;

      std::vector<std::shared_ptr<Graphics::IMaterial>> mMaterials;

      std::vector<std::shared_ptr<Graphics::DynamicMaterial>> mDynamicMaterials;

      std::shared_ptr<TextHandler> mTextHandler;

      std::shared_ptr<InstancedGeometryBatchHolder> mInstancedGeometryBatchHolder;

#ifdef DEBUG
      std::shared_ptr<DebugUiController> mDebugUiController;
#endif

      std::shared_ptr<UiHandler> mUiHandler;

   public:
      explicit Scene(InterThreadCommunicationMgr &interThreadMgr);

      ~Scene();

      std::shared_ptr<Component> CreateComponent_GameThread(const std::shared_ptr<IComponentCreatable> &componentCreator,
                                                            const std::shared_ptr<ComponentData> &componentData);

      void Initialize();

      void OnLevelInit();

      void PostLevelInit();

      void PostPhysicsInitialize();

      void PostPlayLevelFinished();

      void Tick(const float deltaTime) override;

      void UnpausableTick(const float deltaTime) override;

      void ProcessEvent(const WindowSizeChangedGameThreadEvent::EventData_t &data) override;

      void ProcessEvent(const MouseButtonDownRootEvent::EventData_t &data) override;

      void RegisterCamera(const std::shared_ptr<ACamera> &camera);

      void RegisterMainCamera(const std::shared_ptr<ACamera> &camera);

      void UnregisterCamera(const int32_t objectId);

      void UnregisterMainCamera();

      void UnregisterAllCameras();

      void RegisterMaterialInstance(const std::shared_ptr<Graphics::IMaterial> &material);

      std::shared_ptr<EngineObject> GetEngineObjectByName(const std::string &name) const;

      std::shared_ptr<EngineObject> GetEngineObjectById(const int32_t id) const;

      bool RegisterEngineToLuaReplicator(const std::shared_ptr<EngineToLuaReplicatorBase> &replicator);

      bool UnregisterEngineToLuaReplicator(const int32_t replicatorId);

      std::shared_ptr<EngineToLuaReplicatorBase> GetEngineToLuaReplicatorById(const int32_t id) const;

      std::shared_ptr<EngineToLuaReplicatorBase> GetEngineToLuaReplicatorByLuaProxyId(const int32_t id) const;

      std::weak_ptr<IDeferredResourceCreator> GetDeferredResourceCreatorByName(const std::string &name) const;

      const std::vector<std::shared_ptr<ActorController>> &GetActorControllers() const;

      void AddActorController(std::shared_ptr<ActorController> actorController);

      std::shared_ptr<EnginePhysics::PhysicsWorld> GetPhysicsWorld() const;

      const std::vector<std::shared_ptr<Actor>> &GetActors() const;

      std::shared_ptr<Actor> GetActorByName(const std::string &name) const;

      std::shared_ptr<Actor> GetActorById(const int32_t id) const;

      std::shared_ptr<Graphics::IMaterial> GetMaterialByProxyId(const size_t proxyId) const;

      std::shared_ptr<Graphics::IMaterial> GetMaterialByName(const std::string &materialName) const;

      std::shared_ptr<ACamera> GetCamera(const std::string &name) const;

      std::vector<std::shared_ptr<ACamera>> GetActiveCameras() const;

      std::shared_ptr<ACamera> GetMainCamera() const;

      InterThreadCommunicationMgr &GetInterThreadCommunicationManager();

      const std::shared_ptr<TextHandler> &GetTextHandler() const;

      const std::shared_ptr<InstancedGeometryBatchHolder> &GetInstancedGeometryBatchHolder() const;

      std::shared_ptr<UiHandler> GetUiHandler() const;

      void AddActor(std::shared_ptr<Actor> actor);

      void RemoveActor(std::shared_ptr<Actor> actor);

      void RegisterEngineObject(const std::shared_ptr<EngineObject> &gameObject);

      void RemoveEngineObject(const uint32_t objectId);

      void RemoveComponent(std::shared_ptr<Component> component);

      bool RegisterDeferredResourceCreator(const std::shared_ptr<IDeferredResourceCreator> &creatorInstance, const std::string &gameObjectName);

      bool RemoveDeferredResourceCreator(const std::string &gameObjectName);

      glm::vec4 GetConvertedToClippedSpacePosition(const size_t cameraProxyId, const glm::vec4 &worldPosition);

      std::optional<CameraFrustum> GetCameraFrustum(const size_t cameraProxyId);

      void UnloadScene();

#ifdef DEBUG

      void SetRenderThreadFPSTextValue(const float fps);

      void SetGameThreadFPSTextValue(const float fps);

      void SetLuaThreadFPSTextValue(const float fps);
#endif

   private:
      void RegisterComponentSceneProxy(const std::shared_ptr<Component> &component);

      void UnloadUi();

      void UnloadActors();

      void UnloadPhysics();

      void UnloadEngineObjects();

      void UnloadLuaReplicators();

      void UnloadDeferredResourceCreators();

      void UnloadActorControllers();

      void UnloadMaterials();
   };

}
