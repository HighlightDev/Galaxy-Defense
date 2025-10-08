#include "ElectroRayChainFactory.h"

#include "Core/AudioCore/SoundSource.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/StaticMeshComponentCreator.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
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
        "c_electroRayChain_rootComponent_" + rayChainIndexStr, translation, rotation, scale);
    const auto& a_electroRayChain
        = std::make_shared<ElectroRayChainActor>("a_electroRayChain_" + rayChainIndexStr, rootComponent, combatActorsPoolHandler);
    scene->AddActor(a_electroRayChain);

    MaterialParser materialParser;
    const std::shared_ptr<IMaterial>& electro_material = materialParser.ParseMaterialDescriptor("ElectroRayMaterial.m");
    scene->RegisterMaterialInstance(electro_material);
    const auto noiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise.png");

    MaterialPropertySetter::SetMaterialPropertyValue(electro_material, "noise", noiseTex);
    MaterialPropertySetter::SetMaterialPropertyValue(electro_material, "rayColor", glm::vec3(1.0, 1.0, 0.0));
    MaterialPropertySetter::SetMaterialPropertyValue(electro_material, "rayWidthCoef", 1.0f);
    MaterialPropertySetter::SetMaterialPropertyValue(electro_material, scene, "GT_DeltaSec", "gt_timeSec");
    MaterialPropertySetter::SetMaterialPropertyValue(electro_material, a_electroRayChain, "p_opacity", "b_opacity");

    const auto d_mesh = std::make_shared<RuntimeGeneratedMeshComponentData>(
        "c_rayChainRuntineLineMesh_" + rayChainIndexStr, 4, glm::vec3(0), glm::vec3(), glm::vec3(1), "", electro_material);
    const auto& meshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();
    const auto& c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(
        scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
    c_mesh->SetLineWidth(12.0f);
    c_mesh->SetSortOrderValue(100);

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