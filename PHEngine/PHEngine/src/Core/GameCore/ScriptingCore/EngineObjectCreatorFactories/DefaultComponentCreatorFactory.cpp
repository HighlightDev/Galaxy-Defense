#include "DefaultComponentCreatorFactory.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Actor.h"
#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/DirectionalLightComponent.h"
#include "Core/GameCore/Components/PlatformTraverseComponent.h"
#include "Core/GameCore/Components/HumanoidPhysicsMovementComponent.h"
#include "Core/GameCore/Components/ComponentData/DirectionalLightComponentData.h"
#include "Core/GameCore/Components/ComponentData/PlanarReflectionComponentData.h"
#include "Core/GameCore/Components/ComponentData/SpotlightComponentData.h"
#include "Core/GameCore/Components/SpotlightComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/WaterPlaneComponent.h"
#include "Core/GameCore/Components/UiInputComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionBoxShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCapsuleShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionPlaneShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionCompoundShape.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/RigidBodyController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/DynamicCharacterController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Components/PhysicsComponents/RigidBodyPhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/CharacterPhysicsComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PlanarReflectionComponent.h"
#include "Core/GraphicsCore/Shadow/ProjectedDirectionalLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedPointLightShadowInfo.h"
#include "Core/GraphicsCore/Shadow/ProjectedSpotlightShadowInfo.h"
#include "Core/GameCore/Components/ComponentCreators/BillboardComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/CubemapComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/LightComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PlanarReflectionComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PlatformTraverseComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/SkeletalMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/SkyboxComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/UtilityCore/EngineConfigHolder.h"
#include "Core/UtilityCore/JsonUtilities.h"

#include <unordered_map>
#include <json/json.hpp>

using namespace EngineCore;

namespace EngineCore
{
    namespace Scripts
    {
        void DefaultComponentCreatorFactory::CreateComponent(const std::weak_ptr<::EngineCore::Scene> &sceneWp,
                                                             const int32_t actorObjectId,
                                                             const std::string &componentType,
                                                             const std::string &componentDataJsonStr) const
        {
            const auto &sceneSp = sceneWp.lock();
            assert(sceneSp);
            const auto &actor = sceneSp->GetActorById(actorObjectId);
            assert(actor);

            std::unordered_map<std::string, std::shared_ptr<IComponentCreatable>> creatorsMap = {
                {"PointLightComponent", std::make_shared<LightComponentCreator<PointLightComponent>>()},
                {"DirectionalLightComponent", std::make_shared<LightComponentCreator<DirectionalLightComponent>>()},
                {"SpotlightComponent", std::make_shared<LightComponentCreator<SpotlightComponent>>()},
                {"StaticMeshComponent", std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true)},
                {"SkeletalMeshComponent", std::make_shared<SkeletalMeshComponentCreator<SkeletalMeshComponent>>()},
                {"RigidBodyPhysicsComponent", std::make_shared<PhysicsComponentCreator<RigidBodyPhysicsComponent>>()},
                {"CharacterPhysicsComponent", std::make_shared<PhysicsComponentCreator<CharacterPhysicsComponent>>()},
                {"GhostPhysicsComponent", std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>()},
                {"HumanoidPhysicsMovementComponent", std::make_shared<MovementComponentCreator<HumanoidPhysicsMovementComponent>>()},
                {"PlatformTraverseComponent", std::make_shared<PlatformTraverseComponentCreator<PlatformTraverseComponent>>()},
                {"SkyboxComponent", std::make_shared<SkyboxComponentCreator<SkyboxComponent>>()},
                {"WaterPlaneComponent", std::make_shared<StaticMeshComponentCreator<WaterPlaneComponent>>(false)},
                {"PlanarReflectionComponent", std::make_shared<PlanarReflectionComponentCreator<PlanarReflectionComponent>>()},
                {"InputComponent", std::make_shared<InputComponentCreator<InputComponent>>()},
                {"UiInputComponent", std::make_shared<InputComponentCreator<UiInputComponent>>()}};

            assert(creatorsMap.count(componentType));

            const auto componentDataSp = CreateComponentData(sceneSp, componentType, componentDataJsonStr);
            actor->AddComponent(sceneSp->CreateComponent_GameThread(creatorsMap.at(componentType), componentDataSp));
        }

        std::shared_ptr<ComponentData> DefaultComponentCreatorFactory::CreateComponentData(const std::shared_ptr<::EngineCore::Scene> &sceneSp,
                                                                                           const std::string &componentType,
                                                                                           const std::string &componentDataJsonStr) const
        {
            const auto &jsonObj = nlohmann::json::parse(componentDataJsonStr);
            const std::string objectName = nlohmann_utilities::GetStringFromJson(jsonObj["gameObjectName"]);

            std::shared_ptr<ComponentData> componentData;

            if ("PointLightComponent" == componentType)
            {
                const auto translation = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["translation"]);
                const auto ambient = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["ambient"]);
                const auto diffuse = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["diffuse"]);
                const auto specular = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["specular"]);
                const auto attenutation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["attenutation"]);
                const auto radianceRadius = nlohmann_utilities::GetFloatFromJson(jsonObj["radianceRadius"]);
                std::shared_ptr<ProjectedShadowInfo> shadowInfo;
                if (jsonObj.contains("shadowAtlasSize"))
                {
                    const auto shadowAtlasSize = nlohmann_utilities::GetIntFromJson(jsonObj["shadowAtlasSize"]);
                    const auto &pointLightTAR = TextureAtlasFactory::GetInstance()->AddTextureCubeAtlasRequest(glm::ivec2(shadowAtlasSize));
                    shadowInfo = std::make_shared<ProjectedPointLightShadowInfo>(pointLightTAR);
                }

                componentData = std::make_shared<PointLightComponentData>(objectName, translation, attenutation, radianceRadius, ambient, diffuse, specular, shadowInfo);
            }
            else if ("DirectionalLightComponent" == componentType)
            {
                const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
                const auto direction = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["direction"]);
                const auto ambient = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["ambient"]);
                const auto diffuse = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["diffuse"]);
                const auto specular = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["specular"]);
                std::shared_ptr<ProjectedShadowInfo> shadowInfo;
                if (jsonObj.contains("shadowAtlasSize"))
                {
                    const auto &cfg = EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig();
                    const float orthoHalfExtent = cfg.ShadowOrthoProjectionHalfExtent;
                    const auto shadowAtlasSize = nlohmann_utilities::GetIntFromJson(jsonObj["shadowAtlasSize"]);
                    const auto &directionalLightTAR = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(shadowAtlasSize));
                    shadowInfo = std::make_shared<ProjectedDirectionalLightShadowInfo>(directionalLightTAR, orthoHalfExtent);
                }

                componentData = std::make_shared<DirectionalLightComponentData>(objectName, rotation, direction, ambient, diffuse, specular, shadowInfo);
            }
            else if ("SpotlightComponent" == componentType)
            {
                const auto translation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["translation"]);
                const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
                const auto ambient = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["ambient"]);
                const auto diffuse = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["diffuse"]);
                const auto specular = nlohmann_utilities::GetRgbFromJsonMap(jsonObj["specular"]);
                const auto attenutation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["attenutation"]);
                const auto radianceRadius = nlohmann_utilities::GetFloatFromJson(jsonObj["radianceRadius"]);
                const auto cutoff = nlohmann_utilities::GetFloatFromJson(jsonObj["cutoff"]);
                std::shared_ptr<ProjectedShadowInfo> shadowInfo;
                if (jsonObj.contains("shadowAtlasSize"))
                {
                    const auto shadowAtlasSize = nlohmann_utilities::GetIntFromJson(jsonObj["shadowAtlasSize"]);
                    const auto &pointLightTAR = TextureAtlasFactory::GetInstance()->AddTextureAtlasRequest(glm::ivec2(shadowAtlasSize));
                    shadowInfo = std::make_shared<ProjectedSpotlightShadowInfo>(pointLightTAR);
                }

                componentData = std::make_shared<SpotlightComponentData>(objectName, translation, rotation, attenutation, radianceRadius, cutoff, ambient, diffuse, specular, shadowInfo);
            }
            else if ("StaticMeshComponent" == componentType || "SkeletalMeshComponent" == componentType)
            {
                const auto pathToMesh = nlohmann_utilities::GetStringFromJson(jsonObj["meshName"]);
                const auto translation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["translation"]);
                const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
                const auto scale = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["scale"]);
                const auto luaScriptRelPath = nlohmann_utilities::GetStringFromJson(jsonObj["luaScriptName"]);
                const auto materialProxyId = nlohmann_utilities::GetIntFromJson(jsonObj["materialProxyId"]);
                const auto &material = sceneSp->GetMaterialByProxyId(materialProxyId);
                assert(material);

                componentData = std::make_shared<MeshComponentData>(objectName, pathToMesh, translation, rotation, scale, luaScriptRelPath, material);
            }
            else if ("RigidBodyPhysicsComponent" == componentType || "CharacterPhysicsComponent" == componentType || "GhostPhysicsComponent" == componentType)
            {
                std::shared_ptr<PhysicsDescriptor> descriptor;
                std::shared_ptr<CollisionShapeBase> collisionShape;
                const auto collisionShapeStr = nlohmann_utilities::GetStringFromJson(jsonObj["collisionShape"]);
                if ("box" == collisionShapeStr)
                {
                    const auto halfExtent = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["halfExtent"]);
                    collisionShape = std::make_shared<CollisionBoxShape>(halfExtent);
                }
                else if ("capsule" == collisionShapeStr)
                {
                    const auto radius = nlohmann_utilities::GetFloatFromJson(jsonObj["radius"]);
                    const auto height = nlohmann_utilities::GetFloatFromJson(jsonObj["height"]);
                    collisionShape = std::make_shared<CollisionCapsuleShape>(radius, height);
                }
                else if ("plane" == collisionShapeStr)
                {
                    const auto normal = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["normal"]);
                    const auto d = nlohmann_utilities::GetFloatFromJson(jsonObj["d"]);
                    collisionShape = std::make_shared<CollisionPlaneShape>(normal, d);
                }
                else if ("sphere" == collisionShapeStr)
                {
                    const auto radius = nlohmann_utilities::GetFloatFromJson(jsonObj["radius"]);
                    collisionShape = std::make_shared<CollisionSphereShape>(radius);
                }
                else if ("compoundShape" == collisionShapeStr)
                {
                    collisionShape = std::make_shared<CollisionCompoundShape>();
                    // todo:
                    /*void EngineObjectCreator::AddChildShapeToCompoundShape(CollisionShapeBase * compoundShape, CollisionShapeBase * childShape,
                                                                           const glm::vec3 &translation, const glm::vec3 &rotation)
                    {
                        CollisionCompoundShape *mCompoundShape = static_cast<CollisionCompoundShape *>(compoundShape);
                        assert(mCompoundShape);
                        NoScaleEulerRotationTransform childTransform = NoScaleEulerRotationTransform(translation, rotation);
                        mCompoundShape->AddChildShape(childTransform, childShape);
                    }*/
                }

                const auto mass = nlohmann_utilities::GetFloatFromJson(jsonObj["mass"]);
                if ("RigidBodyPhysicsComponent" == componentType)
                {
                    const auto physicsBodyType = static_cast<ePhysicsBodyType>(nlohmann_utilities::GetIntFromJson(jsonObj["physicsBodyType"]));
                    descriptor = std::make_shared<RigidBodyController>(sceneSp->GetPhysicsWorld(), collisionShape, physicsBodyType, mass);
                }
                else if ("CharacterPhysicsComponent" == componentType)
                {
                    const auto capsuleRadius = nlohmann_utilities::GetFloatFromJson(jsonObj["capsuleRadius"]);
                    const auto capsuleHeight = nlohmann_utilities::GetFloatFromJson(jsonObj["capsuleHeight"]);

                    const auto stepHeight = nlohmann_utilities::GetFloatFromJson(jsonObj["stepHeight"]);
                    descriptor = std::make_shared<DynamicCharacterController>(sceneSp->GetPhysicsWorld(), capsuleRadius, capsuleHeight, mass, stepHeight);
                }
                else if ("GhostPhysicsComponent" == componentType)
                {
                    descriptor = std::make_shared<GhostController>(sceneSp->GetPhysicsWorld(), collisionShape, mass);
                }

                componentData = std::make_shared<PhysicsComponentData>(objectName, descriptor);
            }
            else if ("HumanoidPhysicsMovementComponent" == componentType)
            {
                const auto launchDirection = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["launchDirection"]);
                const auto cameraName = nlohmann_utilities::GetStringFromJson(jsonObj["cameraName"]);
                componentData = std::make_shared<HumanoidMovementComponentData>(objectName, launchDirection, cameraName);
            }
            else if ("PlatformTraverseComponent" == componentType)
            {
                const auto scriptName = nlohmann_utilities::GetStringFromJson(jsonObj["scriptName"]);
                componentData = std::make_shared<PlatformTraverseComponentData>(objectName, scriptName);
            }
            else if ("SkyboxComponent" == componentType)
            {
                const auto scale = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["scale"]);
                const auto materialProxyId = nlohmann_utilities::GetIntFromJson(jsonObj["materialProxyId"]);
                const auto &material = sceneSp->GetMaterialByProxyId(materialProxyId);
                assert(material);

                componentData = std::make_shared<SkyboxComponentData>(objectName, scale, material);
            }
            else if ("WaterPlaneComponent" == componentType)
            {
                const auto translation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["translation"]);
                const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
                const auto scale = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["scale"]);
                const auto materialProxyId = nlohmann_utilities::GetIntFromJson(jsonObj["materialProxyId"]);
                const auto &material = sceneSp->GetMaterialByProxyId(materialProxyId);
                assert(material);
                componentData = std::make_shared<MeshComponentData>("", objectName, translation, rotation, scale, "", material);
            }
            else if ("PlanarReflectionComponent" == componentType)
            {
                const auto translation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["translation"]);
                const auto rotation = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["rotation"]);
                const auto scale = nlohmann_utilities::GetXyzFromJsonMap(jsonObj["scale"]);
                const auto cameraName = nlohmann_utilities::GetStringFromJson(jsonObj["cameraName"]);
                const auto ownerCameraSp = sceneSp->GetCamera(cameraName);
                assert(ownerCameraSp);
                const auto viewPortX = nlohmann_utilities::GetIntFromJson(jsonObj["viewPortX"]);
                const auto viewPortY = nlohmann_utilities::GetIntFromJson(jsonObj["viewPortY"]);
                const auto viewPortWidth = nlohmann_utilities::GetIntFromJson(jsonObj["viewPortWidth"]);
                const auto viewPortHeight = nlohmann_utilities::GetIntFromJson(jsonObj["viewPortHeight"]);
                componentData = std::make_shared<PlanarReflectionComponentData>(objectName,
                                                                                translation,
                                                                                rotation,
                                                                                scale,
                                                                                ownerCameraSp,
                                                                                ::Graphics::ViewPortInfo(viewPortX, viewPortY, viewPortWidth, viewPortHeight));
            }
            else if ("InputComponent" == componentType ||
                     "UiInputComponent" == componentType)
            {
                componentData = std::make_shared<ComponentData>(objectName);
            }

            assert(componentData);

            return componentData;
        }
    }
}
