#include "BombMissileFactory.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/ACamera.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/CollisionSphereShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/BombMissileActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Controllers/AiActorController.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t BombMissileFactory::s_bombBulletCounter = 0;

std::shared_ptr<MissileActor> BombMissileFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    const auto& shipBulletIndexStr = std::to_string(s_bombBulletCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_bombMissile_rootComponent_" + shipBulletIndexStr, translation, rotation, scale, true);
    const auto& a_missile
        = std::make_shared<BombMissileActor>("a_bombMissile_" + shipBulletIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_missile);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& pbs_mat = materialParser.ParseMaterialDescriptor("MissileMaterial.m");
    scene->RegisterMaterialInstance(pbs_mat);

    const std::string albedoName = "missile1_albedo.png";
    const std::string normalName = "solar_cells_normal_512.jpg";
    const std::string roughnessName = "solar_cells_roughness_512.jpg";
    const std::string metallicName = "solar_cells_metallic_512.jpg";

    const auto& albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);
    const auto& normal_tex = TexturePool::GetInstance()->GetOrAllocateResource(normalName);
    const auto& roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource(roughnessName);
    const auto& metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource(metallicName);
    const float uvScale = 0.5f;

    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "albedo", albedo_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "normalMap", normal_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "roughnessMap", roughness_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "metallicMap", metallic_tex);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "uvScale", uvScale);
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, scene->GetMainCamera(), "CameraPosition", "cameraPosition");
    MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "outlineColor", glm::vec3(1.0f, 0.0f, 0.0f));

    const auto d_mesh = std::make_shared<MeshComponentData>(
        "c_bombMissileMesh_" + shipBulletIndexStr, "missile1_model.fbx", glm::vec3(0), glm::vec3(0), glm::vec3(1.5), pbs_mat);
    const auto& meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>(true);
    const auto& c_mesh = scene->CreateComponent_GameThread(meshComponentCreator, d_mesh);
    a_missile->AddComponent(c_mesh);

    const auto d_movement
        = std::make_shared<MovementComponentData>("c_bombMissileNoPhysMove_" + shipBulletIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
    const auto& moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
    const auto& c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(
        scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
    c_movement->SetReferenceSpeed(100.0f);
    c_movement->SetCurrentSpeedToReferenceValue();
    c_movement->SetDirection(glm::vec3(.0f, .0f, 1.0f));
    a_missile->AddComponent(c_movement);

    const auto& soundComponentCreator = std::make_shared<AudioComponentCreator<SoundComponent>>();
    const auto& c_sound = std::static_pointer_cast<SoundComponent>(scene->CreateComponent_GameThread(
        soundComponentCreator, std::make_shared<ComponentData>("c_bombMissileSound_" + shipBulletIndexStr)));
    c_sound->CreateSoundBuffer("explosion1.ogg", "explosion");
    c_sound->GetSoundSource()->SetGain(0.2f);
    a_missile->AddComponent(c_sound);

    const auto& ghostController
        = std::make_shared<GhostController>(scene->GetPhysicsWorld(), std::make_shared<CollisionSphereShape>(3.0f), 0.0f);
    const auto& physData = std::make_shared<PhysicsComponentData>("c_bombMissilePhysics_" + shipBulletIndexStr, ghostController);
    const auto& physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
    const auto& c_ghostPhysics = scene->CreateComponent_GameThread(physicsComponentCreator, physData);
    a_missile->AddComponent(c_ghostPhysics);

    const auto& bulletActorController = std::make_shared<AiActorController>(a_missile);
    scene->AddActorController(bulletActorController);

    a_missile->SetScene(scene);

    return a_missile;
}
} // namespace Game