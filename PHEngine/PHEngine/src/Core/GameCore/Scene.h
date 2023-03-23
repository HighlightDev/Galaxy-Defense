#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/ActorController.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/GameCore/Physics/DebugRender/DebugPhysicsRenderData.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Event/eTextEventEnums.h"
#include "Core/ResourceManagerCore/DeferredResources/DeferredResourceCreator.h"
#include "Core/GraphicsCore/SceneViewInfo/CameraFrustum.h"
#include "TextHandler.h"
#include "Core/GameCore/GUI/UiElements/UiHandler.h"
#include "Core/DebugCore/DebugUiController.h"
#include "Core/GameCore/ScriptingCore/EngineToLuaReplicatorBase.h"

#include <type_traits>
#include <glm/vec3.hpp>
#include <optional>

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

   class Scene : public EngineObject,
                 public ITickable,
                 public std::enable_shared_from_this<Scene>

   {
   private:
      EnginePhysics::PhysicsWorld *mPhysicsWorld;

      std::vector<EngineObject *> mEngineObjects;

      std::vector<std::shared_ptr<EngineToLuaReplicatorBase>> mLuaReplicators;

      InterThreadCommunicationMgr &m_interThreadMgr;

      std::shared_ptr<EngineGOProperty<float>> mGameThreadDeltaSec;

      std::unordered_map<std::string, IDeferredResourceCreator *> mDeferredResourceCreators;

      std::vector<std::shared_ptr<Actor>> mActors;

      std::shared_ptr<ACamera> mMainCamera;

      std::vector<std::shared_ptr<ACamera>> mActiveCameras;

      std::vector<std::shared_ptr<ActorController>> mActorControllers;

      std::vector<std::shared_ptr<Graphics::IMaterial>> mMaterials;

      std::vector<std::shared_ptr<Graphics::DynamicMaterial>> mDynamicMaterials;

      std::vector<std::shared_ptr<ITickable>> mExternalTickableObjects;

      TextHandler mTextHandler;

#ifdef DEBUG
      std::unique_ptr<DebugUiController> mDebugUiController;
#endif

      std::shared_ptr<UiHandler> mUiHandler;

   public:
      explicit Scene(InterThreadCommunicationMgr &interThreadMgr);

      ~Scene();

      std::shared_ptr<Component> CreateComponent_GameThread(const std::shared_ptr<IComponentCreatable> &componentCreator,
                                                            const ComponentData &componentData);

      void OnLevelInit();

      void PostLevelInit();

      void PostPhysicsInitialize();

      void PostPlayLevelFinished();

      void Tick(const float deltaTime) override;

      void UnpausableTick(const float deltaTime) override;

      void RegisterCamera(std::shared_ptr<ACamera> camera);

      void RegisterMainCamera(std::shared_ptr<ACamera> camera);

      std::shared_ptr<MaterialProxy> RegisterMaterialInstance(std::shared_ptr<Graphics::IMaterial> material);

      EngineObject *GetEngineObjectByName(const std::string &name) const;

      EngineObject *GetEngineObjectById(const uint64_t id) const;

      bool RegisterEngineToLuaReplicator(const std::shared_ptr<EngineToLuaReplicatorBase> &replicator);

      bool RemoveEngineToLuaReplicator(const std::shared_ptr<EngineToLuaReplicatorBase> &replicator);

      std::shared_ptr<EngineToLuaReplicatorBase> GetEngineToLuaReplicatorById(const int32_t id) const;

      std::shared_ptr<EngineToLuaReplicatorBase> GetEngineToLuaReplicatorByLuaProxyId(const int32_t id) const;

      IDeferredResourceCreator *GetDeferredResourceCreatorByName(const std::string &name) const;

      const std::vector<std::shared_ptr<ActorController>> &GetActorControllers() const;

      void AddActorController(std::shared_ptr<ActorController> actorController);

      EnginePhysics::PhysicsWorld *GetPhysicsWorld() const;

      const std::vector<std::shared_ptr<Actor>> &GetActors() const;

      std::shared_ptr<Actor> GetActorByName(const std::string &name) const;

      std::shared_ptr<Actor> GetActorById(const uint64_t id) const;

      std::shared_ptr<Graphics::IMaterial> GetMaterialByProxyId(const size_t proxyId) const;

      std::shared_ptr<ACamera> GetCamera(const std::string &name) const;

      std::vector<std::shared_ptr<ACamera>> GetActiveCameras() const;

      std::shared_ptr<ACamera> GetMainCamera() const;

      InterThreadCommunicationMgr &GetInterThreadCommunicationManager();

      const TextHandler &GetTextHandler() const;

      std::shared_ptr<UiHandler> GetUiHandler() const;

      void AddActor(std::shared_ptr<Actor> actor);

      void RemoveActor(std::shared_ptr<Actor> actor);

      bool RegisterEngineObject(EngineObject *const gameObjectPtr);

      bool RemoveEngineObject(EngineObject *const gameObjectPtr);

      void AddExternalTickableObject(const std::shared_ptr<ITickable> &externalTickableObject);

      bool UpdatePrimitiveComponentTransform_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId,
                                                            const uint64_t functionId, const glm::mat4 &newRelativeMatrix, const BoundingBox3D &newTransformedBoundingBox);

      bool UpdatePrimitiveComponentEnable_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool bEnabled);

      bool UpdatePrimitiveComponentVisibility_OnRenderThread(const size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool visibility);

      bool UpdatePrimitiveComponentSortOrderValue_OnRenderThread(const size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const int32_t sortOrderValue);

      bool UpdateLightComponentTransform_OnRenderThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4 &newRelativeMatrix);

      bool UpdateCameraSceneProxyData_OnRenderThread(const size_t sceneProxyId, const uint64_t creatorObjectId, const uint64_t functionId, ACamera *camera);

      void RemoveComponent(std::shared_ptr<Component> component);

      void CameraSceneProxyAdded_OnRenderThread(std::shared_ptr<CameraSceneProxy> cameraSceneProxy);

      bool IsCameraSceneProxyExistsOnRT(const size_t sceneProxyId) const;

      void PrimitiveSceneProxyDeleted_OnRenderThread(size_t primitiveSceneProxyIndex);

      void PrimitiveSceneProxyAdded_OnRenderThread(size_t primitiveSceneProxyIndex, std::shared_ptr<PrimitiveSceneProxy> primitiveSceneProxy);

      void PrimitiveSceneProxiesUpdated_OnRenderThread();

      void LightSceneProxyDeleted_OnRenderThread(size_t lightSceneProxyIndex);

      void LightSceneProxyAdded_OnRenderThread(size_t primitiveSceneProxyIndex, std::shared_ptr<LightSceneProxy> lightSceneProxy);

      void LightSceneProxiesUpdated_OnRenderThread();

      void MaterialProxyAdded_OnRenderThread(size_t materialProxyIndex, std::shared_ptr<MaterialProxy> materialProxy);

      void MaterialPropertiesUpdated_OnRenderThread(size_t materialProxyIndex, std::vector<std::shared_ptr<MaterialProperty>> &&properties);

      void PlanarReflectionSceneProxyAdded_OnRenderThread(size_t planarReflectionSceneProxyId, std::shared_ptr<PlanarReflectionProxy> proxy);

      void BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(std::shared_ptr<PlanarReflectionProxy> planarReflectionProxy, ACamera *cameraOwner);

      void RegisterText_OnRenderThread(const std::shared_ptr<HudTextField> &textField, const bool subscribeOnTextScreenSpaceSizeUpdate);

      void UnregisterText_OnRenderThread(const std::shared_ptr<HudTextField> &textField);

      void RegisterUiCanvasProxy_OnRenderThread(const std::shared_ptr<UiCanvasSceneProxy> &uiCanvasProxy);

      void UnregisterUiCanvasProxy_OnRenderThread(const std::shared_ptr<UiCanvasSceneProxy> &uiCanvasProxy);

      void RegisterUiSceneProxy_OnRenderThread(std::shared_ptr<UiSceneProxyBase> uiSceneProxy, const size_t canvasUId);

      void UnregisterUiSceneProxy_OnRenderThread(std::shared_ptr<UiSceneProxyBase> uiSceneProxy, const size_t canvasUId);

      void TextDataChanged_OnRenderThread(const std::shared_ptr<HudTextField> &textField, const eTextChangedDataType textChangedDataType);

      bool RegisterDeferredResourceCreator(IDeferredResourceCreator *creatorInstance, const std::string &gameObjectName);

      bool RemoveDeferredResourceCreator(const std::string &gameObjectName);

      glm::vec4 GetConvertedToClippedSpacePosition(const size_t cameraProxyId, const glm::vec4 &worldPosition);

      std::optional<CameraFrustum> GetCameraFrustum(const size_t cameraProxyId);

#if DEBUG
      void UpdatePhysicsRenderData(const DebugPhysicsRenderData &physRenderData);
#endif

   private:
      void RegisterComponentSceneProxy(const std::shared_ptr<Component> &component);
   };

}
