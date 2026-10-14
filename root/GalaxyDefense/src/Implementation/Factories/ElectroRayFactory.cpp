#include "ElectroRayFactory.h"

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
#include "Implementation/Actors/ElectroRayActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Controllers/AiActorController.h"
#include "Implementation/DataProviders/GameConstants.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t ElectroRayFactory::s_rayCounter = 0;

std::shared_ptr<MissileActor> ElectroRayFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    using namespace Constants::ElectroRay;

    const auto& rayIndexStr = std::to_string(s_rayCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_electroRay_rootComponent_" + rayIndexStr, translation, rotation, scale, true);
    const auto& a_electroRay
        = std::make_shared<ElectroRayActor>("a_electroRay_" + rayIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_electroRay);

    const auto noiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise_128x128.png");
    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& electroRay_material = materialParser.ParseMaterialDescriptor("ElectroBeamMaterial.m");
    scene->RegisterMaterialInstance(electroRay_material);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "noise", noiseTex);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamGlowColor", c_beamGlowColor);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamMainColor", c_beamMainColor);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, scene, "GT_DeltaSec", "gt_timeSec");

    const auto d_mesh = std::make_shared<ElectricBeamComponentData>(
        "c_electricLineMesh_" + rayIndexStr,
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
    c_mesh->SetUpdateFrequency(c_dynamicUpdateFrequency);
    c_mesh->SetJitterAmount(c_dynamicJitterAmount);
    c_mesh->SetAnimationSpeed(c_dynamicAnimationSpeed);
    a_electroRay->SetLineComponent(c_mesh);
    a_electroRay->AddComponent(c_mesh);

    a_electroRay->SetElectroLineOriginSpeed(c_originSpeed);
    a_electroRay->SetElectroLineDestinationSpeed(c_destinationSpeed);

    a_electroRay->SetScene(scene);

    return a_electroRay;
}
} // namespace Game