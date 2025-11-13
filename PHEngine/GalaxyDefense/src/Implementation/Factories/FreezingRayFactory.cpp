#include "FreezingRayFactory.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
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
#include "Implementation/Actors/FreezingRayActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Controllers/AiActorController.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t FreezingRayFactory::s_rayCounter = 0;

std::shared_ptr<MissileActor> FreezingRayFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    const auto& rayIndexStr = std::to_string(s_rayCounter++);
    const auto& rootComponent
        = std::make_shared<EngineCore::SceneComponent>("c_freezingRay_rootComponent_" + rayIndexStr, translation, rotation, scale);
    const auto& a_freezingRay
        = std::make_shared<FreezingRayActor>("a_freezingRay_" + rayIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_freezingRay);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& freezing_material = materialParser.ParseMaterialDescriptor("ElectroRayMaterial.m");
    scene->RegisterMaterialInstance(freezing_material);

    const auto noiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise_128x128.png");

    MaterialPropertySetter::SetMaterialPropertyValue(freezing_material, "noise", noiseTex);
    MaterialPropertySetter::SetMaterialPropertyValue(freezing_material, "rayColor", glm::vec3(0.8, 0.8, 2.0));
    MaterialPropertySetter::SetMaterialPropertyValue(freezing_material, "rayWidthCoef", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(freezing_material, scene, "GT_DeltaSec", "gt_timeSec");
    MaterialPropertySetter::SetMaterialPropertyValue(freezing_material, a_freezingRay, "p_opacity", "b_opacity");

    const auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(
        "c_runtimeLineMesh_" + rayIndexStr, 4, glm::vec3(0), glm::vec3(), glm::vec3(1), "", freezing_material);
    const auto& meshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();
    const auto& c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(
        scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    c_mesh->SetSortOrderValue(101);
    c_mesh->SetDepthWriteMaskEnabled(false);
    a_freezingRay->SetLineComponent(c_mesh);
    a_freezingRay->AddComponent(c_mesh);

    a_freezingRay->SetFreezingRayHitRadius(65.0f);

    c_mesh->SetLineWidth(8.0f);

    a_freezingRay->SetScene(scene);

    return a_freezingRay;
}
} // namespace Game