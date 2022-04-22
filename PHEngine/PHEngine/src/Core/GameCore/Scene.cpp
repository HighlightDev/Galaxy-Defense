#include "Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"

#include <TinyLogger/LogInterface.h>

using namespace Graphics;
using namespace TinyLogger;

namespace EngineCore
{

   Scene::Scene(InterThreadCommunicationMgr &interThreadMgr)
       : GameObject("EngineScene"),
         mPhysicsWorld(new PhysicsWorld()),
         m_interThreadMgr(interThreadMgr),
         mGameThreadDeltaSec(std::make_shared<EngineGOProperty<float>>(0.0f, "GT_DeltaSec")),
         mActiveCameras(),
         mActorControllers()
   {
      Logger::Out("Scene::ctor");

      RegisterGameObject(this);
      AddEngineProperty(mGameThreadDeltaSec);
      mPhysicsWorld->InitPhysicsWorld();
   }

   void Scene::PostLevelInit()
   {
      Logger::Out("Scene::PostLevelInit");

      for (auto &actor : mActors)
      {
         actor->SetScene(shared_from_this());
         actor->PostLevelInit();
      }

      for (auto &camera : mActiveCameras)
      {
         camera->PostLevelInit();
      }
   }

   void Scene::PostPhysicsInitialize()
   {
      Logger::Out("Scene::PostPhysicsInitialize");

      for (auto &actor : mActors)
      {
         actor->PostPhysicsInitialize();
      }
   }

   void Scene::PostPlayLevelFinished()
   {
      Logger::Out("Scene::PostPhysicsInitialize");

      for (auto &actor : mActors)
      {
         actor->PostPlayLevelFinished();
      }
   }

   void Scene::RegisterMainCamera(std::shared_ptr<ACamera> camera)
   {
      Logger::Out("Scene::RegisterMainCamera; name = ", camera->GetCameraName());

      assert(!mMainCamera);
      mMainCamera = camera;
      RegisterCamera(camera);
   }

   void Scene::RegisterCamera(std::shared_ptr<ACamera> camera)
   {
      Logger::Out("Scene::RegisterCamera; name = ", camera->GetCameraName());

      mActiveCameras.emplace_back(camera);
      RegisterGameObject(camera.get());
      auto cameraProxyPtr = camera->CreateSceneProxy();
      camera->SceneProxyId = cameraProxyPtr->GetSceneProxyId();
      CameraSceneProxyAdded_OnRenderThread(cameraProxyPtr);
   }

   std::shared_ptr<MaterialProxy> Scene::RegisterMaterialInstance(std::shared_ptr<IMaterial> material)
   {
      Logger::Out("Scene::RegisterMaterialInstance; name = ", material->MaterialName);

      mMaterials.push_back(material);

      if (material->GetMaterialType() == IMaterial::eMaterialType::DYNAMIC)
      {
         auto dynamicMaterial = std::static_pointer_cast<DynamicMaterial>(material);
         dynamicMaterial->SetScene(shared_from_this());
         mDynamicMaterials.push_back(dynamicMaterial);
      }

      const auto &materialProxy = material->CreateMaterialProxy();
      material->MaterialProxyId = materialProxy->GetSceneProxyId();

      MaterialProxyAdded_OnRenderThread(materialProxy->GetSceneProxyId(), materialProxy);

      return materialProxy;
   }

   const InterThreadCommunicationMgr &Scene::GetThreadManager() const
   {
      return m_interThreadMgr;
   }

   EnginePhysics::PhysicsWorld *Scene::GetPhysicsWorld() const
   {
      return mPhysicsWorld;
   }

   std::shared_ptr<ACamera> Scene::GetCamera(const std::string &cameraName) const
   {
      auto cameraIt = std::find_if(mActiveCameras.begin(), mActiveCameras.end(), [&](const auto &cameraPtr)
                                   { return cameraPtr->GetGameObjectName() == cameraName; });

      if (cameraIt != mActiveCameras.end())
      {
         return (*cameraIt);
      }

      return nullptr;
   }

   std::vector<std::shared_ptr<ACamera>> Scene::GetActiveCameras() const
   {
      return mActiveCameras;
   }

   std::shared_ptr<ACamera> Scene::GetMainCamera() const
   {
      assert(mMainCamera);
      return mMainCamera;
   }

   const std::vector<std::shared_ptr<Actor>> &Scene::GetActors() const
   {
      return mActors;
   }

   std::shared_ptr<Actor> Scene::GetActorByName(const std::string &name) const
   {
      auto foundActor = std::find_if(mActors.begin(), mActors.end(), [&](const auto &actor)
                                     { return actor->GetGameObjectName() == name; });
      assert(foundActor != mActors.end());
      return *foundActor;
   }

   std::shared_ptr<Actor> Scene::GetActorById(const uint64_t id) const
   {
      auto foundActor = std::find_if(mActors.begin(), mActors.end(), [&](const auto &actor)
                                     { return actor->GetObjectId() == id; });
      assert(foundActor != mActors.end());
      return *foundActor;
   }

   std::shared_ptr<IMaterial> Scene::GetMaterialByProxyId(const size_t proxyId) const
   {
      const auto materialIt = std::find_if(mMaterials.begin(), mMaterials.end(), [=](const auto &material)
                                           { return material->MaterialProxyId == proxyId; });
      return materialIt != mMaterials.end() ? *materialIt : nullptr;
   }

   void Scene::AddActor(std::shared_ptr<Actor> actor)
   {
      const std::string &goName = actor->GetGameObjectName();
      RegisterGameObject(actor.get());
      mActors.emplace_back(actor);
   }

   void Scene::RemoveActor(std::shared_ptr<Actor> actor)
   {
      const std::string &goName = actor->GetGameObjectName();
      auto it = std::remove_if(GameObjects.begin(), GameObjects.end(), [&](const auto *gameObject)
                               { return gameObject->GetGameObjectName() == goName; });

      if (it != GameObjects.end())
      {
         GameObjects.erase(it, GameObjects.end());
      }
   }

   void Scene::AddExternalTickableObject(const std::shared_ptr<ITickable> &externalTickableObject)
   {
      mExternalTickableObjects.emplace_back(externalTickableObject);
   }

   GameObject *Scene::GetGameObjectByName(const std::string &name) const
   {
      GameObject *go = nullptr;

      auto it = std::find_if(GameObjects.begin(), GameObjects.end(), [&](const auto *gameObject)
                             { return gameObject->GetGameObjectName() == name; });

      if (it != GameObjects.end())
      {
         go = *it;
      }

      return go;
   }

   GameObject *Scene::GetGameObjectById(const uint64_t id) const
   {
      GameObject *go = nullptr;

      auto it = std::find_if(GameObjects.begin(), GameObjects.end(), [=](const auto *gameObject)
                             { return gameObject->GetObjectId() == id; });

      if (it != GameObjects.end())
      {
         go = *it;
      }

      return go;
   }

   IDeferredResourceCreator *Scene::GetDeferredResourceCreatorByName(const std::string &name) const
   {
      IDeferredResourceCreator *creatorInstance = nullptr;

      if (mDeferredResourceCreators.count(name))
      {
         creatorInstance = mDeferredResourceCreators.at(name);
      }

      return creatorInstance;
   }

   const std::vector<std::shared_ptr<ActorController>> &Scene::GetActorControllers() const
   {
      return mActorControllers;
   }

   void Scene::AddActorController(std::shared_ptr<ActorController> actorController)
   {
      const auto it = std::find_if(mActorControllers.begin(), mActorControllers.end(), [&](const auto &existingController)
                                   { return existingController->GetBindedActorName() == actorController->GetBindedActorName(); });
      assert(it == mActorControllers.end());

      actorController->InitActorController();
      mActorControllers.emplace_back(actorController);
   }

   void Scene::ExecuteOnRenderThread(eEnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, std::function<void(void)> gameThreadJobCallback) const
   {
      m_interThreadMgr.EmplaceRenderThreadJob(policy, Job(creatorObjectId, functionId, gameThreadJobCallback));
   }

   void Scene::ExecuteOnGameThread(eEnqueueJobPolicy policy, const uint64_t creatorObjectId, const uint64_t functionId, std::function<void(void)> renderThreadJobCallback) const
   {
      m_interThreadMgr.EmplaceGameThreadJob(policy, Job(creatorObjectId, functionId, renderThreadJobCallback));
   }

   void Scene::UpdatePrimitiveComponentEnable_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool bEnabled)
   {
      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(
             eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
             Job(creatorObjectId,
                 functionId, [=]()
                 {
                  const auto& primitiveSp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                  if (primitiveSp)
                  {
                     primitiveSp->SetEnabled(bEnabled);
                  }
                  else {
                       Logger::Out("Scene::UpdatePrimitiveComponentEnable_OnRenderThread."
                                       "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                                                                primitiveSceneProxyIndex);
                  } }));
      }
   }

   void Scene::UpdatePrimitiveComponentVisibility_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const bool visibility)
   {
      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(
             eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
             Job(creatorObjectId,
                 functionId, [=]()
                 {
                    const auto& primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                  if (primitiveProxySp)
                  {
                     primitiveProxySp->SetVisibility(visibility);
                  }
                  else {
                       Logger::Out("Scene::UpdatePrimitiveComponentVisibility_OnRenderThread."
                                       "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                                                                primitiveSceneProxyIndex);
                  } }));
      }
   }

   void Scene::UpdatePrimitiveComponentTransform_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId,
                                                                const uint64_t functionId, const glm::mat4 &newRelativeMatrix, const BoundingBox &newTransformedBoundingBox)
   {
      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
                                                 Job(creatorObjectId,
                                                     functionId, [=]()
                                                     {
           const auto& primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                  if (primitiveProxySp)
            {
               primitiveProxySp->SetTransformationMatrix(newRelativeMatrix);
               primitiveProxySp->SetTransformedBoundingBox(newTransformedBoundingBox);
            }
            else
            {
               Logger::Out("Scene::UpdatePrimitiveComponentTransform_OnRenderThread."
                              "Error !Current proxy index doesn't exist on RT. Proxy index = " , primitiveSceneProxyIndex);
            } }));
      }
   }

   void Scene::UpdateCameraSceneProxyData_OnRenderThread(const size_t sceneProxyId, const uint64_t creatorObjectId, const uint64_t functionId, ACamera *camera)
   {
      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         const auto &sceneViewSp = sceneRenderer->GetSceneViewByProxyId(sceneProxyId);
         if (sceneViewSp)
         {
            m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
                                                    Job(creatorObjectId,
                                                        functionId, [=]()
                                                        {

                  sceneViewSp->GetCameraProxy()->UpdateEyeVector(camera->GetEyeVector());
                  sceneViewSp->GetCameraProxy()->UpdateViewMatrix(camera->GetViewMatrix()); }));
         }
         else
         {
            Logger::Out("Scene::UpdateCameraSceneProxyData_OnRenderThread."
                        "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                        sceneProxyId);
         }
      }
   }

   void Scene::UpdateLightComponentTransform_OnRenderThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4 &newRelativeMatrix)
   {
      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
                                                 Job(creatorObjectId,
                                                     functionId, [=]()
                                                     {
               const auto& lightProxySp = sceneRenderer->GetLightProxyByProxyId(lightSceneProxyIndex);  
               if (lightProxySp)
               {
                 lightProxySp->SetTransformationMatrix(newRelativeMatrix);
               }
               else
               {
                  Logger::Out("Scene::UpdateLightComponentTransform_OnRenderThread."
                              "Error! Current proxy index doesn't exist on RT. Proxy index = ", lightSceneProxyIndex);
               } }));
      }
   }

   void Scene::PrimitiveSceneProxyDeleted_OnRenderThread(size_t primitiveSceneProxyIndex)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::PrimitiveSceneProxyDeleted_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     {
               const bool bRemoved = sceneRenderer->RemovePrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                  if (bRemoved)
               {
                  sceneRenderer->SetProxiesAreDirty(true);
               }
               else 
               {
                   Logger::Out("Scene::PrimitiveSceneProxyDeleted_OnRenderThread."
                               "Error! Current proxy index doesn't exist on RT. Proxy index = ", primitiveSceneProxyIndex);
               } }));
      }
   }

   void Scene::PrimitiveSceneProxiesUpdated_OnRenderThread()
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::PrimitiveSceneProxiesUpdated_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->SetProxiesAreDirty(true); }));
      }
   }

   void Scene::LightSceneProxyDeleted_OnRenderThread(size_t lightSceneProxyIndex)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::LightSceneProxyDeleted_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId,
                                                     functionId, [=]()
                                                     {
               const bool bRemoved = sceneRenderer->RemoveLightProxyByProxyId(lightSceneProxyIndex);
               if (bRemoved)
               {
                  sceneRenderer->SetLightProxiesAreDirty(true); 
               }
               else
               {
                  Logger::Out("Scene::LightSceneProxyDeleted_OnRenderThread."
                        "Error! Current proxy index doesn't exist on RT. Proxy index = ", lightSceneProxyIndex);
               } }));
      }
   }

   void Scene::LightSceneProxiesUpdated_OnRenderThread()
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::LightSceneProxiesUpdated_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->SetLightProxiesAreDirty(true); }));
      }
   }

   void Scene::CameraSceneProxyAdded_OnRenderThread(std::shared_ptr<CameraSceneProxy> cameraSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::CameraSceneProxyAdded_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->SceneViewsVector.emplace_back(std::make_shared<SceneView>(cameraSceneProxy, sceneRenderer->PrimitiveProxiesVector)); }));
      }
   }

   void Scene::PrimitiveSceneProxyAdded_OnRenderThread(size_t primitiveSceneProxyIndex, std::shared_ptr<PrimitiveSceneProxy> primitiveSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::PrimitiveSceneProxyAdded_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     {
            const auto& primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
            assert(!primitiveProxySp);
            sceneRenderer->PrimitiveProxiesVector.emplace_back(primitiveSceneProxy);
            sceneRenderer->SetProxiesAreDirty(true); }));
      }
   }

   void Scene::LightSceneProxyAdded_OnRenderThread(size_t lightSceneProxyIndex, std::shared_ptr<LightSceneProxy> lightSceneProxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::LightSceneProxyAdded_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId,
                                                     functionId, [=]()
                                                     {
         const auto& lightProxySp = sceneRenderer->GetLightProxyByProxyId(lightSceneProxyIndex);  
         assert(!lightProxySp);
         sceneRenderer->LightProxiesVector.emplace_back(lightSceneProxy);
         sceneRenderer->SetLightProxiesAreDirty(true); }));
      }
   }

   void Scene::MaterialProxyAdded_OnRenderThread(size_t materialProxyIndex, std::shared_ptr<MaterialProxy> materialProxy)
   {
      Logger::Out("Scene::MaterialProxyAdded_OnRenderThread; material name = ", materialProxy->MaterialName);

      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::MaterialProxyAdded_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { 
                                                        const auto& materialProxySp = sceneRenderer->GetMaterialProxyByProxyId(materialProxyIndex);
                                                        assert(!materialProxySp);
                                                        Logger::Out("MaterialProxyAdded_OnRenderThread::Job material name = ", materialProxy->MaterialName);
                                                        sceneRenderer->MaterialProxiesVector.emplace_back(materialProxy); }));
      }
   }

   void Scene::MaterialPropertiesUpdated_OnRenderThread(size_t materialProxyIndex, std::vector<std::shared_ptr<MaterialProperty>> &&properties)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::MaterialPropertiesUpdated_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
                                                 Job(creatorObjectId, functionId, [=, properties = std::move(properties)]() mutable
                                                     { 
                                                      const auto& materialProxySp = sceneRenderer->GetMaterialProxyByProxyId(materialProxyIndex);
                                                      if (materialProxySp)
                                                      {
                                                         materialProxySp->UpdateProperties(std::move(properties)); 
                                                      }
                                                      else{
                                                         Logger::Out("Scene::MaterialPropertiesUpdated_OnRenderThread."
                                                            "Error! Current proxy index doesn't exist on RT. Proxy index = ", materialProxyIndex);
                                                      } }));
      }
   }

   void Scene::PlanarReflectionSceneProxyAdded_OnRenderThread(size_t planarReflectionSceneProxyId, std::shared_ptr<PlanarReflectionProxy> proxy)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::PlanarReflectionSceneProxyAdded");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     {
            const auto& reflectionProxySp = sceneRenderer->GetPlanarReflectionProxyByProxyId(planarReflectionSceneProxyId);
            assert(!reflectionProxySp);
            sceneRenderer->PlanarReflectionProxiesVector.emplace_back(proxy);
            sceneRenderer->SetPlanarReflectionProxiesAreDirty(true); }));
      }
   }

   void Scene::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(std::shared_ptr<PlanarReflectionProxy> planarReflectionProxy, ACamera *cameraOwner)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         const auto proxyId = cameraOwner->SceneProxyId;

         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     {
         const auto &sceneViewSp = sceneRenderer->GetSceneViewByProxyId(proxyId);
         if (sceneViewSp)
            {
               planarReflectionProxy->SetSceneViewWeakPtr(sceneViewSp);
            }
             else
            {
               Logger::Out("Scene::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread."
                     "Error! Current proxy index doesn't exist on RT. Proxy index = ", proxyId);
            } }));
      }
   }

#if DEBUG
   void Scene::UpdatePhysicsRenderData(const DebugPhysicsRenderData &physRenderData)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::UpdatePhysicsRenderData");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE_AND_PUSH,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->SetDebugPhysicsRenderData(physRenderData); }));
      }
   }
#endif

   void Scene::Tick_GameThread(float delta)
   {
      constexpr float inv_PhysicsStep = 1.0f / 260.0f;
      const float physTickStep = delta * (inv_PhysicsStep / delta);

      mGameThreadDeltaSec->SetValue(delta);

      mPhysicsWorld->Tick(physTickStep);

      for (const auto &cameraPtr : mActiveCameras)
      {
         cameraPtr->Tick(delta);
      }

      for (auto &actor : mActors)
      {
         actor->Tick(delta);
      }

      for (const auto &actorController : mActorControllers)
      {
         actorController->Tick(delta);
      }

      for (auto &dynamicMaterial : mDynamicMaterials)
      {
         dynamicMaterial->Tick(delta);
      }

      for (auto &externalTickable : mExternalTickableObjects)
      {
         externalTickable->Tick(delta);
      }

#if DEBUG
      UpdatePhysicsRenderData(mPhysicsWorld->GetDebugPhysicsRenderData());
#endif
   }

   void Scene::RemoveComponent(std::shared_ptr<Component> component)
   {
      const ComponentType type = component->GetComponentType();

      // Remove corresponding primitive proxy
      if ((type & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<PrimitiveComponent>(component);
         const size_t removeProxyIndex = componentPtr->SceneProxyId;

         // delete light proxy from render thread
         PrimitiveSceneProxyDeleted_OnRenderThread(removeProxyIndex);
      }
      else if ((type & ComponentType::LIGHT_COMPONENT) == ComponentType::LIGHT_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<LightComponent>(component);
         const size_t removeProxyIndex = componentPtr->LightSceneProxyId;

         // delete light proxy from render thread
         LightSceneProxyDeleted_OnRenderThread(removeProxyIndex);
      }

      if (const auto &spOwner = component->GetOwner().lock())
      {
         if ((type & ComponentType::MOVEMENT_COMPONENT) == ComponentType::MOVEMENT_COMPONENT)
         {
            spOwner->RemoveMovementComponent();
         }
         else if ((type & ComponentType::INPUT_COMPONENT) == ComponentType::INPUT_COMPONENT)
         {
            spOwner->RemoveInputComponent();
         }
         else
         {
            spOwner->RemoveComponent(component);
         }
      }

      RemoveGameObject(component.get());
   }

   void Scene::RegisterComponentSceneProxy(const std::shared_ptr<Component> &component)
   {
      Logger::Out("Scene::RegisterComponentSceneProxy; componentName = ", component->GetGameObjectName());

      ComponentType type = component->GetComponentType();
      if ((type & ComponentType::SCENE_COMPONENT) == ComponentType::SCENE_COMPONENT)
      {
         SceneComponent *sceneComponentPtr = static_cast<SceneComponent *>(component.get());
         sceneComponentPtr->SetScene(shared_from_this());
         if ((type & ComponentType::PRIMITIVE_COMPONENT) == ComponentType::PRIMITIVE_COMPONENT)
         {
            PrimitiveComponent *componentPtr = static_cast<PrimitiveComponent *>(sceneComponentPtr);

            auto sceneProxySp = componentPtr->CreateSceneProxy();
            componentPtr->SceneProxyId = sceneProxySp->GetSceneProxyId();
            PrimitiveSceneProxyAdded_OnRenderThread(componentPtr->SceneProxyId, sceneProxySp);
         }
         else if ((type & ComponentType::LIGHT_COMPONENT) == ComponentType::LIGHT_COMPONENT)
         {
            LightComponent *componentPtr = static_cast<LightComponent *>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreateSceneProxy();
            componentPtr->LightSceneProxyId = sceneProxySp->GetSceneProxyId();
            LightSceneProxyAdded_OnRenderThread(componentPtr->LightSceneProxyId, sceneProxySp);
         }
         else if ((type & ComponentType::PLANAR_REFLECTION_COMPONENT) == ComponentType::PLANAR_REFLECTION_COMPONENT)
         {
            PlanarReflectionComponent *componentPtr = static_cast<PlanarReflectionComponent *>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreatePlanarReflectionProxy();
            BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(sceneProxySp, componentPtr->GetOwnerCamera());
            componentPtr->SetSceneProxyId(sceneProxySp->GetSceneProxyId());
            PlanarReflectionSceneProxyAdded_OnRenderThread(componentPtr->GetSceneProxyId(), sceneProxySp);
         }
      }
   }

   bool Scene::RegisterDeferredResourceCreator(IDeferredResourceCreator *creatorInstance, const std::string &gameObjectName)
   {
      // Add deferred resource creator instance
      assert(!mDeferredResourceCreators.count(gameObjectName));
      mDeferredResourceCreators[gameObjectName] = creatorInstance;

      return true;
   }

   bool Scene::RemoveDeferredResourceCreator(const std::string &gameObjectName)
   {
      // Remove deferred resource creator instance
      if (mDeferredResourceCreators.count(gameObjectName))
      {
         mDeferredResourceCreators.erase(gameObjectName);
         return true;
      }

      return false;
   }

   bool Scene::RegisterGameObject(GameObject *const gameObjectPtr)
   {
      const std::string &goName = gameObjectPtr->GetGameObjectName();
      auto it = std::find_if(GameObjects.begin(), GameObjects.end(), [&](const auto *gameObject)
                             { return gameObject->GetGameObjectName() == goName; });

      assert(it == GameObjects.end());

      // Add game object
      GameObjects.emplace_back(gameObjectPtr);

      return true;
   }

   bool Scene::RemoveGameObject(GameObject *const gameObjectPtr)
   {
      const std::string &goName = gameObjectPtr->GetGameObjectName();

      auto it = std::remove_if(GameObjects.begin(), GameObjects.end(), [&](const auto *gameObject)
                               { return gameObject->GetGameObjectName() == goName; });

      if (it != GameObjects.end())
      {
         GameObjects.erase(it, GameObjects.end());
         return true;
      }

      return false;
   }

   Scene::~Scene()
   {
      delete mPhysicsWorld;
   }
}
