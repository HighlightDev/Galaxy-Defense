#include "ElectroRayFactory.h"

#include "Core/GameCore/Scene.h"
#include "Implementation/Controllers/AiActorController.h"
#include "Core/GameCore/Components/NoPhysicsMovementComponent.h"
#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/Components/PrimitiveComponents/RuntimeGeneratedLineComponent.h"
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
#include "Core/GameCore/Components/ComponentCreators/RuntimeGeneratedMeshComponentCreator.h"

#include "Core/ResourceManagerCore/Pool/TexturePool.h"
#include "Implementation/Actors/MissileActor.h"
#include "Implementation/Actors/ElectroRayActor.h"

using namespace Resources;
using namespace EngineCore;
using namespace Graphics;

namespace Game
{
    size_t ElectroRayFactory::s_rayCounter = 0;

    std::shared_ptr<MissileActor>
    ElectroRayFactory::CreateMissile(const std::shared_ptr<::EngineCore::Scene> &scene,
                                     const std::shared_ptr<::EngineCore::Actor> &spawnerActor,
                                     const glm::vec3 &translation,
                                     const glm::vec3 &rotation,
                                     const glm::vec3 &scale)
    {
        const auto &rayIndexStr = std::to_string(s_rayCounter++);
        const auto &rootComponent = std::make_shared<EngineCore::SceneComponent>("c_electroRay_rootComponent_" + rayIndexStr,
                                                                                 translation, rotation, scale);
        const auto &a_electroRay = std::make_shared<ElectroRayActor>("a_electroRay_" + rayIndexStr, rootComponent);
        scene->AddActor(a_electroRay);

        MaterialParser materialParser;
        const auto &electro_material = materialParser.ParseMaterialDescriptor("Test.m");

        const auto noiseTex = TexturePool::GetInstance()->GetOrAllocateResource("perlin_noise.png");

        MaterialPropertySetter::SetMaterialPropertyValue(electro_material, "noise", noiseTex);
        MaterialPropertySetter::SetMaterialPropertyValue(electro_material, scene.get(), "GT_DeltaSec", "gt_timeSec");

        const RuntimeGeneratedMeshComponentData d_mesh("c_runtimeLineMesh_" + rayIndexStr, 4, glm::vec3(0), glm::vec3(), glm::vec3(1), "", electro_material);
        const auto &meshComponentCreator = std::make_shared<RuntimeGeneratedMeshComponentCreator<RuntimeGeneratedLineComponent>>();
        const auto &c_mesh = std::static_pointer_cast<RuntimeGeneratedLineComponent>(scene->CreateComponent_GameThread(meshComponentCreator, d_mesh));
        a_electroRay->SetLineComponent(c_mesh);
        a_electroRay->SetSpawnerSpaceship(spawnerActor);
        a_electroRay->AddComponent(c_mesh);

        a_electroRay->SetElectroLineOriginSpeed(65.0f);
        a_electroRay->SetElectroLineDestinationSpeed(75.0f);

        /*ComponentData d_audio("c_bombMissileSound_" + rayIndexStr);
        const auto &soundComponentCreator = std::make_shared<AudioComponentCreator<SoundComponent>>();
        const auto &c_sound = std::static_pointer_cast<SoundComponent>(scene->CreateComponent_GameThread(soundComponentCreator, d_audio));
        c_sound->CreateSoundBuffer("explosion1.ogg", "explosion");
        c_sound->GetSoundSource()->SetGain(0.2f);
        a_electroRay->AddComponent(c_sound);*/

        return a_electroRay;
    }
}