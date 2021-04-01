#pragma once

#include "Core/GameCore/Actor.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentCreatorFactory.h"
#include "Core/GameCore/Components/LightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/PlayerController.h"
#include "Core/InterThreadCommunicationMgr.h"
#include "Core/GameCore/Physics/DebugRender/DebugPhysicsRenderData.h"
#include "Core/GameCore/ACamera.h"

#include <type_traits>

using namespace Thread;

class Graphics::Proxy::LightSceneProxy;
class Graphics::Proxy::PrimitiveSceneProxy;
class Graphics::MaterialProxy;
class Graphics::PlanarReflectionProxy;
class EnginePhysics::PhysicsWorld;
class Graphics::IMaterial;

namespace Game
{
   class Scene : public std::enable_shared_from_this<Scene>
   {
   public:

      std::unordered_map<std::string, GameObject*> GameObjects;

      class EnginePhysics::PhysicsWorld* mPhysicsWorld;

   private:

      std::unordered_map<std::string, IDeferredResourceCreator*> mDeferredResourceCreators;

      std::vector<std::shared_ptr<Actor>> mActors;

      InterThreadCommunicationMgr& m_interThreadMgr;

      std::shared_ptr<ACamera> mMainCamera;

      std::vector<std::shared_ptr<ACamera>> mActiveCameras;

      std::shared_ptr<PlayerController> mPlayerController;

      std::vector<std::shared_ptr<IMaterial>> mMaterials;

      std::shared_ptr<Scene> mMeSharedPtr;

   public:

      Scene(InterThreadCommunicationMgr& interThreadMgr);

      ~Scene();

      template <ComponentMetaType metaType, typename ComponentT>
      std::shared_ptr<Component> CreateComponent_GameThread(const ComponentData& componentData)
      {
         const auto& component = ComponentCreatorFactory<metaType, ComponentT>::CreateComponent(componentData, this);
         RegisterComponentSceneProxy(component);
         RegisterGameObject(component.get());
         component->OnPostInitialized();

         return component;
      }

      std::shared_ptr<Scene> GetSharedFromMe();

      void SetMeSharedPtr(std::shared_ptr<Scene> meSharedPtr);

      void PostLevelInit();

      void PostPhysicsInitialize();

      void RegisterCamera(std::shared_ptr<ACamera> camera);

      void RegisterMainCamera(std::shared_ptr<ACamera> camera);

      std::shared_ptr<MaterialProxy> RegisterMaterialInstance(std::shared_ptr<IMaterial> material);

      GameObject* GetGameObjectByName(const std::string& name) const;

      IDeferredResourceCreator* GetDeferredResourceCreatorByName(const std::string& name) const;

      std::shared_ptr<PlayerController> GetPlayerController() const;

      const std::vector<std::shared_ptr<Actor>>& GetActors() const;

      std::shared_ptr<IMaterial> GetMaterialByProxyId(const size_t proxyId) const;

      std::shared_ptr<ACamera> GetCamera(const std::string& name) const;

      std::shared_ptr<ACamera> GetMainCamera() const;

      const InterThreadCommunicationMgr& GetThreadManager() const;

      void SetPlayerController(std::shared_ptr<PlayerController> playerController);

      void AddActor(std::shared_ptr<Actor> actor);

      void RemoveActor(std::shared_ptr<Actor> actor);

      void Tick_GameThread(float delta);

      void UpdatePrimitiveComponentTransform_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId,
         const uint64_t functionId, const glm::mat4& newRelativeMatrix, const BoundingBox& newTransformedBoundingBox);

      void UpdatePrimitiveComponentEnable_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool bEnabled);

      void UpdatePrimitiveComponentVisibility_GameThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool visibility);

      void UpdateLightComponentTransform_GameThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4& newRelativeMatrix);

      void UpdateCameraSceneProxyData_GameThread(const size_t sceneProxyId, const uint64_t creatorObjectId, const uint64_t functionId, ACamera* camera);

      void RemoveComponent_GameThread(std::shared_ptr<Component> component);

      void ExecuteOnRenderThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, std::function<void(void)> gameThreadJobCallback) const;

      void ExecuteOnGameThread(EnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, std::function<void(void)> renderThreadJobCallback) const;

      void CameraSceneProxyAdded(std::shared_ptr<CameraSceneProxy> cameraSceneProxy);

      void PrimitiveSceneProxyDeleted(size_t primitiveSceneProxyIndex);

      void PrimitiveSceneProxyAdded(size_t primitiveSceneProxyIndex, std::shared_ptr<PrimitiveSceneProxy> primitiveSceneProxy);

      void PrimitiveSceneProxiesUpdated();

      void LightSceneProxyDeleted(size_t lightSceneProxyIndex);

      void LightSceneProxyAdded(size_t primitiveSceneProxyIndex, std::shared_ptr<LightSceneProxy> lightSceneProxy);

      void LightSceneProxiesUpdated();

      void MaterialProxyAdded(size_t materialProxyIndex, std::shared_ptr<MaterialProxy> materialProxy);

      void PlanarReflectionSceneProxyAdded(size_t planarReflectionSceneProxyId, std::shared_ptr<PlanarReflectionProxy> proxy);

      void BindPlanarReflectionSceneProxyToSceneView(std::shared_ptr<PlanarReflectionProxy> planarReflectionProxy, ACamera* cameraOwner);

      bool RegisterDeferredResourceCreator(IDeferredResourceCreator* creatorInstance, const std::string& gameObjectName);

      bool RemoveDeferredResourceCreator(const std::string& gameObjectName);

#if DEBUG
      void UpdatePhysicsRenderData(const DebugPhysicsRenderData& physRenderData);
#endif

      private:

         void RegisterComponentSceneProxy(std::shared_ptr<Component> component);

         bool RegisterGameObject(GameObject* const gameObjectPtr);

         bool RemoveGameObject(GameObject* const gameObjectPtr);
   };

}

