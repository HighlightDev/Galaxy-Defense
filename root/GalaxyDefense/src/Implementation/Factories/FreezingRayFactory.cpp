#include "FreezingRayFactory.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ElectricBeamComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/MovementComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/PhysicsComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/PhysicsComponentData.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/PhysicsComponents/GhostPhysicsComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/DynamicBeamComponent.h"
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
#include "Implementation/DataProviders/GameConstants.h"
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
    using namespace Constants::FreezingRay;

    const auto& rayIndexStr = std::to_string(s_rayCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_freezingRay_rootComponent_" + rayIndexStr, translation, rotation, scale, true);
    const auto& a_freezingRay
        = std::make_shared<FreezingRayActor>("a_freezingRay_" + rayIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_freezingRay);

    const auto noiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise_128x128.png");
    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& electroRay_material = materialParser.ParseMaterialDescriptor("ElectroBeamMaterial.m");
    scene->RegisterMaterialInstance(electroRay_material);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "noise", noiseTex);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamGlowColor", c_beamGlowColor);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamMainColor", c_beamMainColor);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, scene, "GT_DeltaSec", "gt_timeSec");

    const auto d_mesh = std::make_shared<ElectricBeamComponentData>(
        "c_runtimeElectricMesh_" + rayIndexStr,
        glm::vec3(),
        glm::vec3(),
        c_beamThickness,
        c_beamCount,
        c_beamJitter,
        c_beamUpdateFrequency,
        electroRay_material);
    const std::shared_ptr<IComponentCreatable>& meshComponentCreator
        = std::make_shared<ElectricBeamComponentCreator<DynamicBeamComponent>>();
    const auto& c_mesh
        = std::static_pointer_cast<DynamicBeamComponent>(scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    a_freezingRay->SetLineComponent(c_mesh);
    a_freezingRay->AddComponent(c_mesh);
    a_freezingRay->SetScene(scene);

    return a_freezingRay;
}
} // namespace Game