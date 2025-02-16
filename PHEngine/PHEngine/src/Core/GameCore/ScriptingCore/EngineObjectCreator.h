#pragma once

#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/Component.h"
#include "Core/GameCore/Scene.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewPortInfo.h"
#include "Core/GraphicsCore/SceneViewInfo/ViewProjectionInfo.h"
#include "EngineObjectCreatorFactories/IEngineActorCreatorFactory.h"
#include "EngineObjectCreatorFactories/IEngineComponentCreatorFactory.h"

#include <memory>
#include <string>

using namespace Graphics;
using namespace EnginePhysics;
using namespace EngineCore::Scripts;

namespace EngineCore {
class EngineObjectCreator {
    std::weak_ptr<Scene> mSceneWp;

    std::unordered_map<std::string, std::shared_ptr<IEngineActorCreatorFactory>> mActorCreatorFactoriesMap;

    std::unordered_set<std::string> mDefaultComponentNames;

    std::unordered_map<std::string, std::shared_ptr<IEngineComponentCreatorFactory>> mComponentCreatorFactoriesMap;

public:
    EngineObjectCreator();

    void SetScene(const std::weak_ptr<Scene>& scene);

    void RegisterActorCreatorFactory(
        const std::string& factoryKey, const std::shared_ptr<IEngineActorCreatorFactory>& creatorFactoryInstance);

    void RegisterComponentCreatorFactory(
        const std::string& factoryKey, const std::shared_ptr<IEngineComponentCreatorFactory>& creatorFactoryInstance);

public:
    // Actor will be created and added to the scene
    int32_t CreateActor(
        const std::string& actorType,
        const std::string& actorName,
        const glm::vec3& rootTranslation,
        const glm::vec3& rootEulerRotation,
        const glm::vec3& rootScale,
        const std::string& jsonArgsStr) const;

    void
    CreateComponent(const int32_t actorObjectId, const std::string& componentType, const std::string& componentDataJsonStr) const;

    void CreatePlanarReflectionComponent(const std::string& componentDataJsonStr) const;

    void CreateThirdPersonCamera(
        const std::string& cameraName,
        const ViewPortInfo& viewPort,
        const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
        const float initPitchDeg,
        const float initYawDeg,
        const float camDistanceToThirdPersonTarget,
        const glm::vec3& thirdPersonTargetOffset,
        const bool bIsMainSceneCamera);

    void CreateFirstPersonCamera(
        const std::string& cameraName,
        const ViewPortInfo& viewPort,
        const std::shared_ptr<ViewProjectionInfo>& viewProjectionInfo,
        const float initPitchDeg,
        const float initYawDeg,
        const glm::vec3& cameraPosition,
        const bool bIsMainSceneCamera);

    std::shared_ptr<ViewProjectionInfo> CreateViewProjectionInfo(const std::string& jsonArgs) const;
};

} // namespace EngineCore
