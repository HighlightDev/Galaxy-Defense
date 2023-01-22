#include "Scene.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GraphicsCore/Renderer/DeferredShadingSceneRenderer.h"
#include "Core/GraphicsCore/Material/IMaterial.h"
#include "Core/GraphicsCore/Material/DynamicMaterial.h"
#include "Core/GraphicsCore/SceneProxy/PlanarReflectionProxy.h"
#include "Core/GameCore/GUI/Common/TextFieldProxy.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/Components/ComponentCreators/IComponentCreatable.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GameCore/LoggerExtension.h"
#include "Core/GraphicsCore/UiSceneProxy/UiSceneProxyBase.h"
#include "Core/GameCore/GUI/Common/TextFieldProxyType.h"

using namespace Graphics;
using namespace TinyLogger;

namespace EngineCore
{

   Scene::Scene(InterThreadCommunicationMgr &interThreadMgr)
       : EngineObject("EngineScene"),
         mPhysicsWorld(new PhysicsWorld()),
         EngineObjects(),
         m_interThreadMgr(interThreadMgr),
         mGameThreadDeltaSec(std::make_shared<EngineGOProperty<float>>(0.0f, "GT_DeltaSec")),
         mDeferredResourceCreators(),
         mActors(),
         mMainCamera(),
         mActiveCameras(),
         mActorControllers(),
         mMaterials(),
         mDynamicMaterials(),
         mExternalTickableObjects(),
         mTextHandler(),
#ifdef DEBUG
         mDebugUiController(std::make_unique<DebugUiController>()),
#endif
         mUiHandler(std::make_shared<UiHandler>())
   {
      LogInfo("Scene::ctor");

      RegisterEngineObject(this);
      AddEngineProperty(mGameThreadDeltaSec);
      mPhysicsWorld->InitPhysicsWorld();
   }

   void Scene::OnLevelInit()
   {
      LogInfo("Scene::OnLevelInit");
   }

   void Scene::PostLevelInit()
   {
      LogInfo("Scene::PostLevelInit");

      mTextHandler.SetScene(shared_from_this());
      mUiHandler->SetScene(shared_from_this());
#ifdef DEBUG
      mDebugUiController->SetScene(shared_from_this());
#endif

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
      LogInfo("Scene::PostPhysicsInitialize");

      for (auto &actor : mActors)
      {
         actor->PostPhysicsInitialize();
      }
   }

   void Scene::PostPlayLevelFinished()
   {
      LogInfo("Scene::PostPhysicsInitialize");

      for (auto &actor : mActors)
      {
         actor->PostPlayLevelFinished();
      }

      mDebugUiController->PostPlayLevelFinished();
   }

   void Scene::RegisterMainCamera(std::shared_ptr<ACamera> camera)
   {
      LogInfo("Scene::RegisterMainCamera => name = ", camera->GetCameraName());

      assert(!mMainCamera);
      mMainCamera = camera;
      RegisterCamera(camera);
   }

   void Scene::RegisterCamera(std::shared_ptr<ACamera> camera)
   {
      LogInfo("Scene::RegisterCamera => name = ", camera->GetCameraName());

      mActiveCameras.emplace_back(camera);
      RegisterEngineObject(camera.get());
      auto cameraProxyPtr = camera->CreateSceneProxy();
      camera->SceneProxyId = cameraProxyPtr->GetSceneProxyId();
      CameraSceneProxyAdded_OnRenderThread(cameraProxyPtr);
   }

   std::shared_ptr<MaterialProxy> Scene::RegisterMaterialInstance(std::shared_ptr<IMaterial> material)
   {
      LogInfo("Scene::RegisterMaterialInstance => name = ", material->MaterialName);

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
                                   { return cameraPtr->GetEngineObjectName() == cameraName; });

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
                                     { return actor->GetEngineObjectName() == name; });
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
      const std::string &goName = actor->GetEngineObjectName();
      RegisterEngineObject(actor.get());
      mActors.emplace_back(actor);
   }

   void Scene::RemoveActor(std::shared_ptr<Actor> actor)
   {
      const std::string &goName = actor->GetEngineObjectName();
      auto it = std::remove_if(EngineObjects.begin(), EngineObjects.end(), [&](const auto *gameObject)
                               { return gameObject->GetEngineObjectName() == goName; });

      if (it != EngineObjects.end())
      {
         EngineObjects.erase(it, EngineObjects.end());
      }
   }

   void Scene::AddExternalTickableObject(const std::shared_ptr<ITickable> &externalTickableObject)
   {
      mExternalTickableObjects.emplace_back(externalTickableObject);
   }

   EngineObject *Scene::GetEngineObjectByName(const std::string &name) const
   {
      EngineObject *go = nullptr;

      auto it = std::find_if(EngineObjects.begin(), EngineObjects.end(), [&](const auto *gameObject)
                             { return gameObject->GetEngineObjectName() == name; });

      if (it != EngineObjects.end())
      {
         go = *it;
      }

      return go;
   }

   EngineObject *Scene::GetEngineObjectById(const uint64_t id) const
   {
      EngineObject *go = nullptr;

      auto it = std::find_if(EngineObjects.begin(), EngineObjects.end(), [=](const auto *gameObject)
                             { return gameObject->GetObjectId() == id; });

      if (it != EngineObjects.end())
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

   const TextHandler &Scene::GetTextHandler() const
   {
      return mTextHandler;
   }

   std::shared_ptr<UiHandler> Scene::GetUiHandler() const
   {
      return mUiHandler;
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
             eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
             Job(creatorObjectId,
                 functionId, [=]()
                 {
                  const auto& primitiveSp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                  if (primitiveSp)
                  {
                     primitiveSp->SetEnabled(bEnabled);
                  }
                  else {
                       LogInfo("Scene::UpdatePrimitiveComponentEnable_OnRenderThread => "
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
             eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
             Job(creatorObjectId,
                 functionId, [=]()
                 {
                    const auto& primitiveProxySp = sceneRenderer->GetPrimitiveProxyByProxyId(primitiveSceneProxyIndex);
                  if (primitiveProxySp)
                  {
                     primitiveProxySp->SetVisibility(visibility);
                  }
                  else {
                       LogInfo("Scene::UpdatePrimitiveComponentVisibility_OnRenderThread => "
                                       "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                                                                primitiveSceneProxyIndex);
                  } }));
      }
   }

   void Scene::UpdatePrimitiveComponentTransform_OnRenderThread(size_t primitiveSceneProxyIndex, const uint64_t creatorObjectId,
                                                                const uint64_t functionId, const glm::mat4 &newRelativeMatrix, const BoundingBox3D &newTransformedBoundingBox)
   {
      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
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
               LogInfo("Scene::UpdatePrimitiveComponentTransform_OnRenderThread => "
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
            m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                                                    Job(creatorObjectId,
                                                        functionId, [=]()
                                                        {
                                                           sceneViewSp->GetCameraProxy()->UpdateEyeVector(camera->GetEyeVector());
                                                           sceneViewSp->GetCameraProxy()->UpdateViewMatrix(camera->GetViewMatrix());

                                                           m_interThreadMgr.EmplaceGameThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                                                                                                 Job(creatorObjectId,
                                                                                                     functionId, [=]()
                                                                                                     { camera->OnCameraSceneProxyDataUpdated(); })); }));
         }
         else
         {
            LogInfo("Scene::UpdateCameraSceneProxyData_OnRenderThread => "
                    "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                    sceneProxyId);
         }
      }
   }

   bool Scene::IsCameraSceneProxyExistsOnRT(const size_t sceneProxyId) const
   {
      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         const auto &sceneViewSp = sceneRenderer->GetSceneViewByProxyId(sceneProxyId);
         return sceneViewSp != nullptr;
      }

      return false;
   }

   void Scene::UpdateLightComponentTransform_OnRenderThread(size_t lightSceneProxyIndex, const uint64_t creatorObjectId, const uint64_t functionId, const glm::mat4 &newRelativeMatrix)
   {
      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
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
                  LogInfo("Scene::UpdateLightComponentTransform_OnRenderThread => "
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
                   LogInfo("Scene::PrimitiveSceneProxyDeleted_OnRenderThread => "
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
                  LogInfo("Scene::LightSceneProxyDeleted_OnRenderThread => "
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
      LogInfo("Scene::MaterialProxyAdded_OnRenderThread => material name = ", materialProxy->MaterialName, "proxyId = ", materialProxyIndex);

      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::MaterialProxyAdded_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { 
                                                        const auto& materialProxySp = sceneRenderer->GetMaterialProxyByProxyId(materialProxyIndex);
                                                        assert(!materialProxySp);
                                                        LogInfo("MaterialProxyAdded_OnRenderThread::Job => material name = ", materialProxy->MaterialName);
                                                        sceneRenderer->MaterialProxiesVector.emplace_back(materialProxy); }));
      }
   }

   void Scene::RegisterText_OnRenderThread(const std::shared_ptr<HudTextField> &textField, const bool subscribeOnTextScreenSpaceSizeUpdate)
   {
      LogInfo("Scene::RegisterText_OnRenderThread => font name = ", textField->GetFontName(), " textFieldId = ", textField->GetTextFieldId());

      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::RegisterText_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->RegisterText(TextFieldProxy::CreateTextFieldProxyInstance(
                                                           textField->GetTextFieldId(),
                                                           eTextFieldProxyType::HUD_TEXT_FIELD,
                                                           textField->GetIsVisible(),
                                                           textField->GetText(),
                                                           textField->GetFontName(),
                                                           textField->GetPosition(),
                                                           textField->GetColor(),
                                                           textField->GetFontSize(),
                                                           textField->GetTextHorizontalAlignment(),
                                                           textField->GetLineMaxSize(),
                                                           textField->GetNumberOfLines(),
                                                           subscribeOnTextScreenSpaceSizeUpdate)); }));
      }
   }

   void Scene::UnregisterText_OnRenderThread(const std::shared_ptr<HudTextField> &textField)
   {
      LogInfo("Scene::UnregisterText_OnRenderThread => font name = ", textField->GetFontName(), " textFieldId = ", textField->GetTextFieldId());

      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::UnregisterText_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->UnregisterText(textField->GetFontName(), textField->GetTextFieldId()); }));
      }
   }

   void Scene::RegisterUiCanvasProxy_OnRenderThread(const std::shared_ptr<UiCanvasSceneProxy> &uiCanvasProxy)
   {
      LogInfo("Scene::RegisterUiCanvasProxy_OnRenderThread => UId = ", uiCanvasProxy->GetUiItemUId());

      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash64_CT("Scene::RegisterUiCanvasProxy_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->RegisterUiCanvasProxy(uiCanvasProxy); }));
      }
   }

   void Scene::UnregisterUiCanvasProxy_OnRenderThread(const std::shared_ptr<UiCanvasSceneProxy> &uiCanvasProxy)
   {
      LogInfo("Scene::UnregisterUiCanvasProxy_OnRenderThread => UId = ", uiCanvasProxy->GetUiItemUId());

      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash64_CT("Scene::UnregisterUiCanvasProxy_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->UnregisterUiCanvasProxy(uiCanvasProxy); }));
      }
   }

   void Scene::RegisterUiSceneProxy_OnRenderThread(const std::shared_ptr<UiSceneProxyBase> &uiSceneProxy, const size_t canvasUId)
   {
      LogInfo("Scene::RegisterUiSceneProxy_OnRenderThread => UId = ", uiSceneProxy->GetUiItemUId(), " canvasUId = ", canvasUId);

      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash64_CT("Scene::RegisterUiSceneProxy_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->RegisterUiSceneProxy(uiSceneProxy, canvasUId); }));
      }
   }

   void Scene::UnregisterUiSceneProxy_OnRenderThread(const std::shared_ptr<UiSceneProxyBase> &uiSceneProxy, const size_t canvasUId)
   {
      LogInfo("Scene::UnregisterUiSceneProxy_OnRenderThread => UId = ", uiSceneProxy->GetUiItemUId(), " canvasUId = ", canvasUId);

      static constexpr uint64_t creatorObjectId = 0;
      static constexpr uint64_t functionId = Hash64_CT("Scene::UnregisterUiSceneProxy_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->UnregisterUiSceneProxy(uiSceneProxy, canvasUId); }));
      }
   }

   void Scene::TextDataChanged_OnRenderThread(const std::shared_ptr<HudTextField> &textField, const eTextChangedDataType textChangedDataType)
   {
      const uint64_t creatorObjectId = textField->GetTextFieldId();
      static const uint64_t functionId = Hash("Scene::TextDataChanged_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::PUSH_ANYWAY,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     {
                                                        if (eTextChangedDataType::OFFSET == textChangedDataType)
                                                        {
                                                         sceneRenderer->TextPositionChanged(textField->GetFontName(),
                                                                                          textField->GetTextFieldId(),
                                                                                          textField->GetPosition());
                                                        }
                                                        else if (eTextChangedDataType::COLOR == textChangedDataType)
                                                        {
                                                         sceneRenderer->TextColorChanged(textField->GetFontName(),
                                                                                          textField->GetTextFieldId(),
                                                                                          textField->GetColor());
                                                        }
                                                        else if (eTextChangedDataType::TEXT == textChangedDataType)
                                                        {
                                                           sceneRenderer->TextChanged(textField->GetFontName(),
                                                                                       textField->GetTextFieldId(),
                                                                                       textField->GetText());
                                                        }
                                                        else if (eTextChangedDataType::VISIBILITY == textChangedDataType)
                                                        {
                                                           sceneRenderer->TextVisibilityChanged(textField->GetFontName(),
                                                                                                textField->GetTextFieldId(),
                                                                                                textField->GetIsVisible());
                                                        } }));
      }
   }

   void Scene::MaterialPropertiesUpdated_OnRenderThread(size_t materialProxyIndex, std::vector<std::shared_ptr<MaterialProperty>> &&properties)
   {
      static constexpr uint64_t creatorObjectId = 0;
      static const uint64_t functionId = Hash("Scene::MaterialPropertiesUpdated_OnRenderThread");

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                                                 Job(creatorObjectId, functionId, [=, properties = std::move(properties)]() mutable
                                                     { 
                                                      const auto& materialProxySp = sceneRenderer->GetMaterialProxyByProxyId(materialProxyIndex);
                                                      if (materialProxySp)
                                                      {
                                                         materialProxySp->UpdateProperties(std::move(properties)); 
                                                      }
                                                      else{
                                                         LogInfo("Scene::MaterialPropertiesUpdated_OnRenderThread => "
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
               LogInfo("Scene::BindPlanarReflectionSceneProxyToSceneView_OnRenderThread => "
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
         m_interThreadMgr.EmplaceRenderThreadJob(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE,
                                                 Job(creatorObjectId, functionId, [=]()
                                                     { sceneRenderer->SetDebugPhysicsRenderData(physRenderData); }));
      }
   }
#endif

   void Scene::Tick(const float delta)
   {
      mGameThreadDeltaSec->SetValue(delta);

      mPhysicsWorld->Tick(delta);

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

      mDebugUiController->Tick(delta);
#endif

      mUiHandler->Tick(delta);
   }

   void Scene::UnpausableTick(const float deltaTime)
   {
      mPhysicsWorld->UnpausableTick(deltaTime);

      for (const auto &cameraPtr : mActiveCameras)
      {
         cameraPtr->UnpausableTick(deltaTime);
      }

      for (auto &actor : mActors)
      {
         actor->UnpausableTick(deltaTime);
      }

      for (const auto &actorController : mActorControllers)
      {
         actorController->UnpausableTick(deltaTime);
      }

      for (auto &dynamicMaterial : mDynamicMaterials)
      {
         dynamicMaterial->UnpausableTick(deltaTime);
      }

      for (auto &externalTickable : mExternalTickableObjects)
      {
         externalTickable->UnpausableTick(deltaTime);
      }

#if DEBUG
      mDebugUiController->UnpausableTick(deltaTime);
#endif

      mUiHandler->UnpausableTick(deltaTime);
   }

   void Scene::RemoveComponent(std::shared_ptr<Component> component)
   {
      const eComponentType type = component->GetComponentType();

      // Remove corresponding primitive proxy
      if ((type & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<PrimitiveComponent>(component);
         const size_t removeProxyIndex = componentPtr->SceneProxyId;

         // delete light proxy from render thread
         PrimitiveSceneProxyDeleted_OnRenderThread(removeProxyIndex);
      }
      else if ((type & eComponentType::LIGHT_COMPONENT) == eComponentType::LIGHT_COMPONENT)
      {
         auto componentPtr = std::static_pointer_cast<LightComponent>(component);
         const size_t removeProxyIndex = componentPtr->LightSceneProxyId;

         // delete light proxy from render thread
         LightSceneProxyDeleted_OnRenderThread(removeProxyIndex);
      }

      if (const auto &spOwner = component->GetOwner().lock())
      {
         if ((type & eComponentType::MOVEMENT_COMPONENT) == eComponentType::MOVEMENT_COMPONENT)
         {
            spOwner->RemoveMovementComponent();
         }
         else if ((type & eComponentType::INPUT_COMPONENT) == eComponentType::INPUT_COMPONENT)
         {
            spOwner->RemoveInputComponent();
         }
         else
         {
            spOwner->RemoveComponent(component);
         }
      }

      RemoveEngineObject(component.get());
   }

   void Scene::RegisterComponentSceneProxy(const std::shared_ptr<Component> &component)
   {
      LogInfo("Scene::RegisterComponentSceneProxy => componentName = ", component->GetEngineObjectName());

      const eComponentType type = component->GetComponentType();
      if ((type & eComponentType::SCENE_COMPONENT) == eComponentType::SCENE_COMPONENT)
      {
         SceneComponent *sceneComponentPtr = static_cast<SceneComponent *>(component.get());
         if ((type & eComponentType::PRIMITIVE_COMPONENT) == eComponentType::PRIMITIVE_COMPONENT)
         {
            PrimitiveComponent *componentPtr = static_cast<PrimitiveComponent *>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreateSceneProxy();
            componentPtr->SceneProxyId = sceneProxySp->GetSceneProxyId();
            PrimitiveSceneProxyAdded_OnRenderThread(componentPtr->SceneProxyId, sceneProxySp);
            LogInfo("Scene::RegisterComponentSceneProxy => primitive proxy added, sceneProxyId =", componentPtr->SceneProxyId);
         }
         else if ((type & eComponentType::LIGHT_COMPONENT) == eComponentType::LIGHT_COMPONENT)
         {
            LightComponent *componentPtr = static_cast<LightComponent *>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreateSceneProxy();
            componentPtr->LightSceneProxyId = sceneProxySp->GetSceneProxyId();
            LightSceneProxyAdded_OnRenderThread(componentPtr->LightSceneProxyId, sceneProxySp);
            LogInfo("Scene::RegisterComponentSceneProxy => ligth proxy added, sceneProxyId =", componentPtr->LightSceneProxyId);
         }
         else if ((type & eComponentType::PLANAR_REFLECTION_COMPONENT) == eComponentType::PLANAR_REFLECTION_COMPONENT)
         {
            PlanarReflectionComponent *componentPtr = static_cast<PlanarReflectionComponent *>(sceneComponentPtr);
            auto sceneProxySp = componentPtr->CreatePlanarReflectionProxy();
            BindPlanarReflectionSceneProxyToSceneView_OnRenderThread(sceneProxySp, componentPtr->GetOwnerCamera());
            componentPtr->SetSceneProxyId(sceneProxySp->GetSceneProxyId());
            PlanarReflectionSceneProxyAdded_OnRenderThread(componentPtr->GetSceneProxyId(), sceneProxySp);
            LogInfo("Scene::RegisterComponentSceneProxy => planar reflection proxy added, sceneProxyId =", componentPtr->GetSceneProxyId());
         }
      }
   }

   std::shared_ptr<Component> Scene::CreateComponent_GameThread(const std::shared_ptr<IComponentCreatable> &componentCreator,
                                                                const ComponentData &componentData)
   {
      const auto component = componentCreator->CreateComponent(shared_from_this(), componentData);
      component->SetScene(shared_from_this());
      RegisterComponentSceneProxy(component);
      RegisterEngineObject(component.get());
      component->OnPostInitialized();
      return component;
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

   bool Scene::RegisterEngineObject(EngineObject *const gameObjectPtr)
   {
      const std::string &goName = gameObjectPtr->GetEngineObjectName();
      auto it = std::find_if(EngineObjects.begin(), EngineObjects.end(), [&](const auto *gameObject)
                             { return gameObject->GetEngineObjectName() == goName; });

      assert(it == EngineObjects.end());

      // Add game object
      EngineObjects.emplace_back(gameObjectPtr);

      return true;
   }

   bool Scene::RemoveEngineObject(EngineObject *const gameObjectPtr)
   {
      const std::string &goName = gameObjectPtr->GetEngineObjectName();

      auto it = std::remove_if(EngineObjects.begin(), EngineObjects.end(), [&](const auto *gameObject)
                               { return gameObject->GetEngineObjectName() == goName; });

      if (it != EngineObjects.end())
      {
         EngineObjects.erase(it, EngineObjects.end());
         return true;
      }

      return false;
   }

   glm::vec4 Scene::GetConvertedToClippedSpacePosition(const size_t cameraProxyId, const glm::vec4 &worldPosition)
   {
      glm::vec4 result = worldPosition;

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         const auto &sceneViewSp = sceneRenderer->GetSceneViewByProxyId(cameraProxyId);
         if (sceneViewSp)
         {
            const auto &cameraProxySp = sceneViewSp->GetCameraProxy();
            const auto &viewMatrix = cameraProxySp->GetViewMatrix();
            const auto &projectionMatrix = cameraProxySp->GetProjectionMatrix();
            result = projectionMatrix * viewMatrix * result;
         }
         else
         {
            LogInfo("Scene::GetConvertedToClippedSpacePosition => "
                    "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                    cameraProxyId);
         }
      }

      return result;
   }

   std::optional<CameraFrustum> Scene::GetCameraFrustum(const size_t cameraProxyId)
   {
      std::optional<CameraFrustum> result(std::nullopt);

      if (const auto &sceneRenderer = m_interThreadMgr.TryGetSceneRendererWP().lock())
      {
         const auto &sceneViewSp = sceneRenderer->GetSceneViewByProxyId(cameraProxyId);
         if (sceneViewSp)
         {
            if (sceneViewSp->GetCameraProxy()->IsCameraFrustumBuilt())
            {
               result = sceneViewSp->GetCameraProxy()->GetCameraFrustum();
            }
            else
            {
               LogInfo("Scene::GetCameraFrustum => "
                       "Error! Camera Frustum wasn't built yet. Proxy index = ",
                       cameraProxyId);
            }
         }
         else
         {
            LogInfo("Scene::GetCameraFrustum => "
                    "Error! Current proxy index doesn't exist on RT. Proxy index = ",
                    cameraProxyId);
         }
      }

      return result;
   }

   Scene::~Scene()
   {
      delete mPhysicsWorld;
   }
}
