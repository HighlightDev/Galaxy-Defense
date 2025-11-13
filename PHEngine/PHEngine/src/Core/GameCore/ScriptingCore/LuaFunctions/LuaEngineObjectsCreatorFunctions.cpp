#include "LuaEngineObjectsCreatorFunctions.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/FirstPersonCamera.h"
#include "Core/GameCore/HumanoidPlayerController.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreatorFactories/ActorControllerCreatorFactory.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreatorFactories/ActorCreatorFactory.h"
#include "Core/GameCore/ScriptingCore/EngineObjectCreatorFactories/DefaultComponentCreatorFactory.h"
#include "Core/GameCore/ScriptingCore/LuaBindingHelper.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/GameCore/ThirdPersonCamera.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/IoCore/AsyncLoaderCore/ResourceMap.h"
#include "Core/IoCore/FolderManager.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Core/UtilityCore/JsonUtilities.h"

using namespace Graphics;
using namespace EngineUtility;
using namespace IO;
using namespace Resources;
using namespace EngineCore::Scripts;

namespace EngineCore {
namespace Scripts {
LuaEngineObjectsCreatorFunctions::LuaEngineObjectsCreatorFunctions(LuaScriptExecutorBase* ownerPtr)
    : mOwnerPtr(ownerPtr)
    , mSceneWp()
    , mEngineObjectCreator(std::make_shared<EngineObjectCreator>())
{
}

LuaEngineObjectsCreatorFunctions::~LuaEngineObjectsCreatorFunctions()
{
}

void LuaEngineObjectsCreatorFunctions::Initialize()
{
}

void LuaEngineObjectsCreatorFunctions::SetScene(const std::weak_ptr<Scene>& sceneWp)
{
    mSceneWp = sceneWp;

    mEngineObjectCreator->SetScene(mSceneWp);
    mEngineObjectCreator->RegisterActorCreatorFactory("Actor", std::make_shared<ActorCreatorFactory>());
    mEngineObjectCreator->RegisterComponentCreatorFactory(
        "DefaultComponentCreatorFactory", std::make_shared<DefaultComponentCreatorFactory>());
    mEngineObjectCreator->RegisterActorControllerCreatorFactory(
        "DefaultActorControllerCreatorFactory", std::make_shared<ActorControllerCreatorFactory>());
}

void LuaEngineObjectsCreatorFunctions::SetLuaScriptProcessor(const std::weak_ptr<LuaScriptProcessor>& scriptProcessor)
{
    mLuaScriptProcessor = scriptProcessor;
}

std::shared_ptr<EngineObjectCreator> LuaEngineObjectsCreatorFunctions::GetEngineObjectCreator() const
{
    return mEngineObjectCreator;
}

void LuaEngineObjectsCreatorFunctions::OnScriptStarted(const LuaWrapper& luaWrapper)
{
}

void LuaEngineObjectsCreatorFunctions::OnScriptStopped(const LuaWrapper& luaWrapper)
{
}

void LuaEngineObjectsCreatorFunctions::RegisterCallbacks(const LuaWrapper& luaWrapper)
{
    LuaCallbackBindingHelper<Hash64_CT("LuaEngineObjectsCreatorFunctions::LazyLoadResourcesAsync"), void(std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaEngineObjectsCreatorFunctions::LazyLoadResourcesAsync, this, std::placeholders::_1),
        "_LazyLoadResourcesAsync");
    LuaCallbackBindingHelper<Hash64_CT("LuaEngineObjectsCreatorFunctions::OpenAudioStreams"), void(std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaEngineObjectsCreatorFunctions::OpenAudioStreams, this, std::placeholders::_1),
        "_OpenAudioStreams");

    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::CreateActor"),
        int32_t(std::string, std::string, glm::vec3, glm::vec3, glm::vec3, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::CreateActor, this, std::placeholders::_1),
            "_CreateActor");
    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::CreateAndAttachComponentToActor"),
        void(int32_t /*actorObjectId*/, std::string /*componentType*/, std::string /*component data json*/)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::CreateAndAttachComponentToActor, this, std::placeholders::_1),
            "_CreateAndAttachComponentToActor");

    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::CreatePlanarReflectionComponent"),
        void(std::string /*component data json*/)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::CreatePlanarReflectionComponent, this, std::placeholders::_1),
            "_CreatePlanarReflectionComponent");

    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::CreateThirdPersonCamera"),
        void(std::string, glm::ivec4, std::string, float, float, float, glm::vec3, int32_t)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::CreateThirdPersonCamera, this, std::placeholders::_1),
            "_CreateThirdPersonCamera");

    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::SetCameraThirdPersonTarget"),
        void(std::string, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::SetCameraThirdPersonTarget, this, std::placeholders::_1),
            "_SetCameraThirdPersonTarget");

    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::CreateFirstPersonCamera"),
        void(std::string, glm::ivec4, std::string, float, float, glm::vec3, int32_t)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::CreateFirstPersonCamera, this, std::placeholders::_1),
            "_CreateFirstPersonCamera");

    LuaCallbackBindingHelper<Hash64_CT("LuaEngineObjectsCreatorFunctions::CreateMaterial"), int32_t(std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaEngineObjectsCreatorFunctions::CreateMaterial, this, std::placeholders::_1),
        "_CreateMaterial");
    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::SetTextureToMaterial"),
        void(int32_t, std::string, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::SetTextureToMaterial, this, std::placeholders::_1),
            "_SetTextureToMaterial");
    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::SetFloatToMaterial"),
        void(int32_t, float, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::SetFloatToMaterial, this, std::placeholders::_1),
            "_SetFloatToMaterial");
    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::SetDeferredTextureToMaterial"),
        void(int32_t, std::string, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::SetDeferredTextureToMaterial, this, std::placeholders::_1),
            "_SetDeferredTextureToMaterial");
    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::SetBindingToMaterial"),
        void(int32_t, std::string, std::string, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::SetBindingToMaterial, this, std::placeholders::_1),
            "_SetBindingToMaterial");
    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::SetVec3ToMaterial"),
        void(int32_t, std::string, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::SetVec3ToMaterial, this, std::placeholders::_1),
            "_SetVec3ToMaterial");

    LuaCallbackBindingHelper<Hash64_CT("LuaEngineObjectsCreatorFunctions::CreateTweener"), int32_t(int32_t, std::string)>::Bind(
        luaWrapper,
        mOwnerPtr,
        std::bind(&LuaEngineObjectsCreatorFunctions::CreateTweener, this, std::placeholders::_1),
        "_CreateTweener");
    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::SetTweenerBinding"),
        void(int32_t, int32_t, std::string, std::string, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::SetTweenerBinding, this, std::placeholders::_1),
            "_SetTweenerBinding");

    LuaCallbackBindingHelper<
        Hash64_CT("LuaEngineObjectsCreatorFunctions::CreateActorController"),
        void(std::string, std::string, std::string, std::string)>::
        Bind(
            luaWrapper,
            mOwnerPtr,
            std::bind(&LuaEngineObjectsCreatorFunctions::CreateActorController, this, std::placeholders::_1),
            "_CreateActorController");
}

/* -------------------  Load asynchronously resources by names ----------------------------*/
void LuaEngineObjectsCreatorFunctions::LazyLoadResourcesAsync(const std::tuple<std::string>& dataNames)
{
    const std::string& resourcesNamesStr = std::get<0>(dataNames);
    assert(!resourcesNamesStr.empty());

    const std::vector<std::string>& resourceNames = Split(resourcesNamesStr, ',');

    for (std::string resName : resourceNames) {
        resName = Trim(resName);
        ResourceMap::GetInstance()->AllocateAsync(resName);
    }

    ResourceMap::GetInstance()->WaitUntilResourcesLoad();
}

void LuaEngineObjectsCreatorFunctions::OpenAudioStreams(const std::tuple<std::string>& dataNames)
{
    const std::string& audioNamesStr = std::get<0>(dataNames);
    assert(!audioNamesStr.empty());

    const std::vector<std::string>& audioNames = Split(audioNamesStr, ',');
    for (std::string audioName : audioNames) {
        audioName = Trim(audioName);
        ResourceMap::GetInstance()->OpenAudioStream(audioName);
    }
}

/* -------------------  Create Actor ----------------------------*/
int32_t LuaEngineObjectsCreatorFunctions::CreateActor(const std::tuple<
                                                      std::string /*Actor type*/,
                                                      std::string /*Actor name*/,
                                                      glm::vec3 /*root component translation*/,
                                                      glm::vec3 /*root component rotation*/,
                                                      glm::vec3 /*root component scale*/,
                                                      std::string /*json params string*/>& actorData)
{
    const auto actorTypeName = std::get<0>(actorData);
    const auto actorName = std::get<1>(actorData);
    const auto rootComponentTranslation = std::get<2>(actorData);
    const auto rootComponentRotation = std::get<3>(actorData);
    const auto rootComponentScale = std::get<4>(actorData);
    const auto jsonParamsString = std::get<5>(actorData);

    return mEngineObjectCreator->CreateActor(
        actorTypeName, actorName, rootComponentTranslation, rootComponentRotation, rootComponentScale, jsonParamsString);
}

/* -------------------  Create component and attach to actor ----------------------------*/
void LuaEngineObjectsCreatorFunctions::CreateAndAttachComponentToActor(
    const std::tuple<int32_t /*actorObjectId*/, std::string /*componentType*/, std::string /*component data json*/>&
        componentData)
{
    mEngineObjectCreator->CreateComponent(std::get<0>(componentData), std::get<1>(componentData), std::get<2>(componentData));
}

void LuaEngineObjectsCreatorFunctions::CreatePlanarReflectionComponent(
    const std::tuple<std::string /*component data json*/>& componentData)
{
    mEngineObjectCreator->CreatePlanarReflectionComponent(std::get<0>(componentData));
}

/* -------------------  Create first person camera ----------------------------*/
void LuaEngineObjectsCreatorFunctions::CreateFirstPersonCamera(const std::tuple<
                                                               std::string /*cameraName*/,
                                                               glm::ivec4 /*viewPort*/,
                                                               std::string /*viewProjectionJsonArgs*/,
                                                               float /*initPitchDeg*/,
                                                               float /*initYawDeg*/,
                                                               glm::vec3 /*init camera position*/,
                                                               int32_t /*is main camera in the scene*/>& cameraData)
{
    const glm::ivec4& viewPortData = std::get<1>(cameraData);
    const bool bIsMainSceneCamera = static_cast<int32_t>(std::get<6>(cameraData));

    const auto& viewProjectionInfo = mEngineObjectCreator->CreateViewProjectionInfo(std::get<2>(cameraData));

    mEngineObjectCreator->CreateFirstPersonCamera(
        std::get<0>(cameraData),
        ViewPortInfo(viewPortData.x, viewPortData.y, viewPortData.z, viewPortData.w),
        viewProjectionInfo,
        std::get<3>(cameraData),
        std::get<4>(cameraData),
        std::get<5>(cameraData),
        bIsMainSceneCamera);
}

/* -------------------  Create third person camera ----------------------------*/
void LuaEngineObjectsCreatorFunctions::CreateThirdPersonCamera(const std::tuple<
                                                               std::string /*cameraName*/,
                                                               glm::ivec4 /*viewPort*/,
                                                               std::string /*viewProjectionJsonArgs*/,
                                                               float /*initPitchDeg*/,
                                                               float /*initYawDeg*/,
                                                               float /*camDistanceToThirdPersonTarget*/,
                                                               glm::vec3 /*thirdPersonTargetOffset*/,
                                                               int32_t /*is main camera in the scene*/>& cameraData)
{
    const glm::ivec4& viewPortData = std::get<1>(cameraData);
    const bool bIsMainSceneCamera = static_cast<int32_t>(std::get<7>(cameraData));

    const auto& viewProjectionInfo = mEngineObjectCreator->CreateViewProjectionInfo(std::get<2>(cameraData));

    mEngineObjectCreator->CreateThirdPersonCamera(
        std::get<0>(cameraData),
        ViewPortInfo(viewPortData.x, viewPortData.y, viewPortData.z, viewPortData.w),
        viewProjectionInfo,
        std::get<3>(cameraData),
        std::get<4>(cameraData),
        std::get<5>(cameraData),
        std::get<6>(cameraData),
        bIsMainSceneCamera);
}

/* -------------------  Set third person target ----------------------------*/
void LuaEngineObjectsCreatorFunctions::SetCameraThirdPersonTarget(
    const std::tuple<std::string /*camera name*/, std::string /*third person actor name*/>& cameraData)
{
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto& cameraSp = std::dynamic_pointer_cast<ThirdPersonCamera>(sceneSp->GetCamera(std::get<0>(cameraData)));
    assert(cameraSp);
    cameraSp->SetThirdPersonTargetDeferred(std::get<1>(cameraData));
}

/* -------------------  Create material instance and register --------------------*/
int32_t LuaEngineObjectsCreatorFunctions::CreateMaterial(const std::tuple<std::string>& buildMaterial)
{
    MaterialParser materialParser;
    const std::string& materialName = std::get<0>(buildMaterial);
    const std::shared_ptr<IMaterial>& material = materialParser.ParseMaterialDescriptor(materialName);
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    sceneSp->RegisterMaterialInstance(material);
    return material->MaterialProxyId;
}

/* -------------------  Set texture --------------------*/
void LuaEngineObjectsCreatorFunctions::SetTextureToMaterial(
    const std::tuple<int32_t /*material proxy id*/, std::string, std::string>& setTextureToMaterial)
{
    const auto materialId = std::get<0>(setTextureToMaterial);
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto materialSp = sceneSp->GetMaterialByProxyId(materialId);
    assert(materialSp);
    const std::string& textureNames = std::get<1>(setTextureToMaterial);
    const std::string& propertyName = std::get<2>(setTextureToMaterial);
    const auto& texture = TexturePool::GetInstance()->GetOrAllocateResource(textureNames);
    MaterialPropertySetter::SetMaterialPropertyValue(materialSp, propertyName, texture);
}

/* -------------------  Set deferred texture --------------------*/
void LuaEngineObjectsCreatorFunctions::SetDeferredTextureToMaterial(const std::tuple<
                                                                    int32_t /*material proxy id*/,
                                                                    /*deferred resource creator name*/ std::string,
                                                                    /*property name*/ std::string>& data)
{
    const auto materialId = std::get<0>(data);
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto materialSp = sceneSp->GetMaterialByProxyId(materialId);
    assert(materialSp);
    const std::string& resourceCreatorName = std::get<1>(data);
    const std::string& propertyName = std::get<2>(data);

    if (auto scene = mSceneWp.lock()) {
        const auto resourceCreatorWp = scene->GetDeferredResourceCreatorByName(resourceCreatorName);
        if (const auto resourceCreatorSp = resourceCreatorWp.lock()) {
            MaterialPropertySetter::SetMaterialPropertyValue(materialSp, propertyName, resourceCreatorSp);
        }
    }
}

/* -------------------  Set float --------------------*/
void LuaEngineObjectsCreatorFunctions::SetFloatToMaterial(
    const std::tuple<int32_t /*material proxy id*/, float, std::string>& setFloatValueToMaterial)
{
    const auto materialId = std::get<0>(setFloatValueToMaterial);
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto materialSp = sceneSp->GetMaterialByProxyId(materialId);
    assert(materialSp);
    float value = std::get<1>(setFloatValueToMaterial);
    const std::string& propertyName = std::get<2>(setFloatValueToMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(materialSp, propertyName, value);
}

/* -------------------  Set binding to material --------------------*/
void LuaEngineObjectsCreatorFunctions::SetBindingToMaterial(
    const std::tuple<int32_t /*material proxy id*/, std::string, std::string, std::string>& setBindingToMaterial)
{
    const auto materialId = std::get<0>(setBindingToMaterial);
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto materialSp = sceneSp->GetMaterialByProxyId(materialId);
    assert(materialSp);
    const std::string& gameObjectName = std::get<1>(setBindingToMaterial);
    const std::string& gamePropertyName = std::get<2>(setBindingToMaterial);
    const std::string& bindingName = std::get<3>(setBindingToMaterial);

    const auto& gameObject = sceneSp->GetEngineObjectByName(gameObjectName);
    MaterialPropertySetter::SetMaterialPropertyValue(materialSp, gameObject, gamePropertyName, bindingName);
}

void LuaEngineObjectsCreatorFunctions::SetVec3ToMaterial(
    const std::tuple<int32_t /*material proxy id*/, std::string /*value*/, std::string /*property name*/>& setVec3ToMaterial)
{
    const auto materialId = std::get<0>(setVec3ToMaterial);
    const auto sceneSp = mSceneWp.lock();
    assert(sceneSp);
    const auto materialSp = sceneSp->GetMaterialByProxyId(materialId);
    assert(materialSp);
    const auto& jsonObj = nlohmann::json::parse(std::get<1>(setVec3ToMaterial));
    const glm::vec3 value = nlohmann_utilities::GetXyzFromJsonMap(jsonObj);
    const std::string& propertyName = std::get<2>(setVec3ToMaterial);
    MaterialPropertySetter::SetMaterialPropertyValue(materialSp, propertyName, value);
}

/* -------------------  Create Tweener ----------------------------*/
int32_t
LuaEngineObjectsCreatorFunctions::CreateTweener(const std::tuple<int32_t /*Actor id*/, std::string /*tweener name*/>& tweenerData)
{
    const auto actorId = std::get<0>(tweenerData);

    if (const auto& sceneSp = mSceneWp.lock()) {
        const auto actorSp = sceneSp->GetActorById(actorId);
        assert(actorSp);
        TweenerParser fsmParser;
        const auto& tweener = fsmParser.ParseTweenerDescriptor(std::get<1>(tweenerData));
        assert(tweener);
        actorSp->AttachTweener(tweener);
        return tweener->GetId();
    }

    return -1;
}

/* -------------------  Set tweener bindings ------------------------*/
void LuaEngineObjectsCreatorFunctions::SetTweenerBinding(const std::tuple<
                                                         int32_t /*Actor id which holds tweener*/,
                                                         int32_t /*tweener id*/,
                                                         std::string /*game object name*/,
                                                         std::string /*binding name*/,
                                                         std::string /*property name*/>& tweenerData)
{
    const auto actorId = std::get<0>(tweenerData);
    const auto tweenerId = std::get<1>(tweenerData);
    if (const auto& sceneSp = mSceneWp.lock()) {
        const auto actorSp = sceneSp->GetActorById(actorId);
        assert(actorSp);
        const auto tweenerSp = actorSp->GetTweenerById(tweenerId);
        assert(tweenerSp);
        const auto& gameObjectSp = sceneSp->GetEngineObjectByName(std::get<2>(tweenerData));
        const auto& bindingSp = tweenerSp->GetPropertyBindingByName(std::get<3>(tweenerData));
        BindingAttachmentBuilder::SetAttachment(gameObjectSp, bindingSp, std::get<4>(tweenerData));
    }
}

/* -------------------  Create Actor Controller----------------------------*/
void LuaEngineObjectsCreatorFunctions::CreateActorController(const std::tuple<
                                                             std::string /*Factory type name*/,
                                                             std::string /*Actor name*/,
                                                             std::string /*Actor controller type name*/,
                                                             std::string /*json params string*/>& data)
{
    const std::string& factoryTypeName = std::get<0>(data);
    const auto& actorName = std::get<1>(data);
    const auto& actorControllerTypeName = std::get<2>(data);
    const auto& jsonParams = std::get<3>(data);
    mEngineObjectCreator->CreateActorController(factoryTypeName, actorName, actorControllerTypeName, jsonParams);
}
} // namespace Scripts
} // namespace EngineCore
