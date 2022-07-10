#include "BlackHoleMissileFactory.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/GhostController.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/Shapes/PhySphereShape.h"
#include "Core/GameCore/Physics/PhysicsWorld.h"
#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"

#include "Implementation/Controllers/AiActorController.h"
#include "Implementation/Actors/BlackHoleMissileActor.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game
{
    size_t BlackHoleMissileFactory::s_blackHoleMissileCounter = 0;

    std::shared_ptr<Actor>
    BlackHoleMissileFactory::CreateWeaponBullet(const std::shared_ptr<::EngineCore::Scene> &scene,
                                                const glm::vec3 &translation,
                                                const glm::vec3 &rotation,
                                                const glm::vec3 &scale)
    {
        const auto &missileIndexStr = std::to_string(s_blackHoleMissileCounter++);
        const auto &rootComponent = std::make_shared<EngineCore::SceneComponent>("c_missile_rootComponent_" + missileIndexStr,
                                                                                 translation, rotation, scale);
        const auto &a_missile = std::make_shared<BlackHoleMissileActor>("a_missile_" + missileIndexStr, rootComponent);
        scene->AddActor(a_missile);

        const auto &a_missileCombatActivePhase = std::make_shared<Actor>("a_missileCombatActivePhase_" + missileIndexStr,
                                                                         std::make_shared<EngineCore::SceneComponent>("c_missileCombatActivePhase_rootComponent_" + missileIndexStr,
                                                                                                                      glm::vec3(), glm::vec3(), glm::vec3(1)));

        MaterialParser materialParser;
        const auto &pbs_mat = materialParser.ParseMaterialDescriptor("PhysicalBasedMaterial.m");

        const std::string albedoName = "missile1_albedo.png";
        const std::string normalName = "solar_cells_normal_512.jpg";
        const std::string roughnessName = "solar_cells_roughness_512.jpg";
        const std::string metallicName = "solar_cells_metallic_512.jpg";

        const auto &albedo_tex = TexturePool::GetInstance()->GetOrAllocateResource(albedoName);
        const auto &normal_tex = TexturePool::GetInstance()->GetOrAllocateResource(normalName);
        const auto &roughness_tex = TexturePool::GetInstance()->GetOrAllocateResource(roughnessName);
        const auto &metallic_tex = TexturePool::GetInstance()->GetOrAllocateResource(metallicName);
        const float uvScale = 0.5f;

        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "albedo", albedo_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "normalMap", normal_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "roughnessMap", roughness_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "metallicMap", metallic_tex);
        MaterialPropertySetter::SetMaterialPropertyValue(pbs_mat, "uvScale", uvScale);

        const MeshComponentData d_mesh("c_meshComponent_" + missileIndexStr, "missile1_model.fbx", glm::vec3(0),
                                       glm::vec3(0), glm::vec3(1.5), "", pbs_mat);
        const auto &meshComponentCreator = std::make_shared<StaticMeshComponentCreator<StaticMeshComponent>>();
        const auto &c_mesh = scene->CreateComponent_GameThread(meshComponentCreator, d_mesh);
        a_missileCombatActivePhase->AddComponent(c_mesh);

        a_missile->AddChild(a_missileCombatActivePhase);

        ComponentData d_audio("c_soundComponent_" + missileIndexStr);
        const auto &soundComponentCreator = std::make_shared<AudioComponentCreator<SoundComponent>>();
        const auto &c_sound = std::static_pointer_cast<SoundComponent>(scene->CreateComponent_GameThread(soundComponentCreator, d_audio));
        c_sound->CreateSoundBuffer("explosion1.ogg", "explosion");
        c_sound->GetSoundSource()->SetGain(0.2f);
        a_missile->AddComponent(c_sound);

        GhostController *ghostController = new GhostController(scene->GetPhysicsWorld(), new PhySphereShape(3.0f), 0.0f);
        scene->GetPhysicsWorld()->AddPhysDescriptor(ghostController);
        PhysicsComponentData physData("c_missilePhysicsComponent_" + missileIndexStr, ghostController);
        const auto &physicsComponentCreator = std::make_shared<PhysicsComponentCreator<GhostPhysicsComponent>>();
        const auto &c_ghostPhysics = scene->CreateComponent_GameThread(physicsComponentCreator, physData);
        a_missileCombatActivePhase->AddComponent(c_ghostPhysics);

        MovementComponentData d_movement("c_noPhysMoveComponent_" + missileIndexStr, glm::vec3(0.0f, 0.0f, -1.0f));
        const auto &moveComponentCreator = std::make_shared<MovementComponentCreator<NoPhysicsMovementComponent>>();
        const auto &c_movement = std::static_pointer_cast<NoPhysicsMovementComponent>(scene->CreateComponent_GameThread(moveComponentCreator, d_movement));
        c_movement->SetSpeed(0.03f);
        c_movement->SetDirection(glm::vec3(.0f, .0f, 1.0f));
        a_missileCombatActivePhase->AddComponent(c_movement);

        const auto &bulletActorController = std::make_shared<AiActorController>(a_missileCombatActivePhase);
        scene->AddActorController(bulletActorController);

        TweenerParser tweenerParser;
        const auto &missileTweener = tweenerParser.ParseTweenerDescriptor("blackHoleMissile.tween");

        a_missile->AttachTweener(missileTweener);

        const auto &binding = missileTweener->GetPropertyBindingByName("b_isEnabled");
        BindingAttachmentBuilder::SetAttachment(a_missileCombatActivePhase.get(), binding.get(), "p_isVisible");

        return a_missile;
    }
}