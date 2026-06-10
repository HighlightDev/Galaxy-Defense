#include "ElectroRayChainFactory.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ElectricBeamComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/DynamicBeamComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/Tweener/BindingAttachmentBuilder.h"
#include "Core/GameCore/Tweener/Tweener.h"
#include "Core/GameCore/Tweener/TweenerParser.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/ElectroRayChainActor.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Levels/CombatLevel/CombatActorsPoolHandler.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game {
size_t ElectroRayChainFactory::s_electroRayChainCounter = 0;

std::shared_ptr<MissileActor> ElectroRayChainFactory::CreateMissile(
    const std::shared_ptr<::EngineCore::Scene>& scene,
    const std::shared_ptr<CombatActorsPoolHandler>& combatActorsPoolHandler,
    const glm::vec3& translation,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    const auto& rayChainIndexStr = std::to_string(s_electroRayChainCounter++);
    const auto& rootComponent = std::make_shared<EngineCore::SceneComponent>(
        "c_electroRayChain_rootComponent_" + rayChainIndexStr, translation, rotation, scale, true);
    const auto& a_electroRayChain
        = std::make_shared<ElectroRayChainActor>("a_electroRayChain_" + rayChainIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_electroRayChain);

    const auto noiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise_128x128.png");
    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& electroRay_material = materialParser.ParseMaterialDescriptor("ElectroBeamMaterial.m");
    scene->RegisterMaterialInstance(electroRay_material);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "noise", noiseTex);
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamGlowColor", glm::vec3(0.8, 0.2, 0.8));
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, "beamMainColor", glm::vec3(0.8, 1.0, 0.2));
    MaterialPropertySetter::SetMaterialPropertyValue(electroRay_material, scene, "GT_DeltaSec", "gt_timeSec");

    const auto d_mesh = std::make_shared<ElectricBeamComponentData>(
        "c_rayChainElectricLineMesh_" + rayChainIndexStr, glm::vec3(), glm::vec3(), 1.0f, 2, 0.7f, 0.05f, electroRay_material);
    const std::shared_ptr<IComponentCreatable>& meshComponentCreator
        = std::make_shared<ElectricBeamComponentCreator<DynamicBeamComponent>>();
    const auto& c_mesh
        = std::static_pointer_cast<DynamicBeamComponent>(scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    c_mesh->SetUpdateFrequency(0.05f);
    c_mesh->SetJitterAmount(0.3f);
    c_mesh->SetAnimationSpeed(2.0f);

    TweenerParser tweenerParser;
    const auto& rayChainTweener = tweenerParser.ParseTweenerDescriptor("electroRayChain.tween");
    a_electroRayChain->AttachTweener(rayChainTweener);
    const auto& b_rayOpacity = rayChainTweener->GetPropertyBindingByName("b_opacity");
    BindingAttachmentBuilder::SetAttachment(a_electroRayChain, b_rayOpacity, "p_opacity");

    a_electroRayChain->SetLineComponent(c_mesh);
    a_electroRayChain->AddComponent(c_mesh);
    a_electroRayChain->SetScene(scene);

    return a_electroRayChain;
}
} // namespace Game