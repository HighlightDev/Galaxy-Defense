#include "TestFeaturesLevel.h"

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/AudioComponents/StreamingSoundComponent.h"
#include "Core/GameCore/Components/ComponentCreators/AudioComponentCreator.h"
#include "Core/GameCore/Components/ComponentCreators/ParticleSystemComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/ParticleSystemComponentData.h"
#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/Event/GameThreadEventDispatcher.h"
#include "Core/GameCore/Event/LuaThreadEventDispatcher.h"
#include "Core/GameCore/Particles/Emitters/ParticleExplosionEmitter.h"
#include "Core/GameCore/Particles/Modules/Color/SimpleColorModule.h"
#include "Core/GameCore/Particles/Modules/Lifetime/SimpleLifeTimeModule.h"
#include "Core/GameCore/Particles/Modules/Size/SimpleSizeModule.h"
#include "Core/GameCore/Particles/Modules/Velocity/SimpleVelocityModule.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaEngineScriptExecutor.h"
#include "Core/GraphicsCore/Material/MaterialParser.h"
#include "Core/GraphicsCore/Material/MaterialProperties/MaterialPropertySetter.h"
#include "Core/UtilityCore/EngineConfigHolder.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace Graphics;
using namespace EnginePhysics;
using namespace IO;
using namespace EngineCore;
using namespace EngineCore::Scripts;
using namespace Resources;
using namespace Graphics;

namespace TestFeatures {

TestFeaturesLevel::TestFeaturesLevel()
    : Level("TestFeaturesLevel")
    , mAmbientMusicDummy(std::make_shared<Actor>(
          "Ambient Music Dummy",
          std::make_shared<SceneComponent>("AmbientMusicDummyRootComponent", glm::vec3(), glm::vec3(), glm::vec3(), true)))

{
}

TestFeaturesLevel::~TestFeaturesLevel()
{
}

void TestFeaturesLevel::PreLevelInit()
{
    Base::PreLevelInit();
    const auto sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "TestFeaturesLevel::PreLevelInit: sceneSp is null");
    // mUiController = std::make_unique<TestFeaturesLevelUiController>(sceneSp);
    if (mUiController) {
        mUiController->OnPreLevelInit();
    }
}

void TestFeaturesLevel::CreateScene()
{
    RunLuaBuildLevelScript();
}

void TestFeaturesLevel::RunLuaBuildLevelScript()
{
    const auto sceneSp = mSceneWp.lock();
    ext_assert(sceneSp, "TestFeaturesLevel::RunLuaBuildLevelScript: sceneSp is null");
    LuaEngineScriptExecutor mLuaLevelBuilder = LuaEngineScriptExecutor("createTestLevel.lua");
    mLuaLevelBuilder.SetScene(sceneSp);
    mLuaLevelBuilder.SetLuaScriptProcessor(sceneSp->GetInterThreadCommunicationManager().GetLuaScriptProcessor());
    mLuaLevelBuilder.RegisterCallbacks();
    mLuaLevelBuilder.RunScript();
    mLuaLevelBuilder.StopScript();
}

std::unique_ptr<::EngineCore::InputComponent> mInputComponent;

void TestFeaturesLevel::PostLevelInit()
{
    Base::PostLevelInit();

    if (const auto& sceneSp = mSceneWp.lock()) {
        mInputComponent = std::make_unique<InputComponent>(std::make_shared<ComponentData>("GameFlowController_InputComponent"));
    }

#ifdef DEBUG
    if (EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig().EnableAmbientMusic) {
        if (const auto& sceneSp = mSceneWp.lock()) {
            sceneSp->AddActor(mAmbientMusicDummy);
            mAmbientMusicDummy->SetScene(mSceneWp);
            const auto musicComponentCreator = std::make_shared<AudioComponentCreator<StreamingSoundComponent>>();
            const auto c_streamingMusic = std::static_pointer_cast<StreamingSoundComponent>(
                sceneSp->CreateComponent_GameThread(musicComponentCreator, std::make_shared<ComponentData>("c_ambientMusic")));
            c_streamingMusic->CreateStreamingSoundSource("piano-loop2.ogg");
            c_streamingMusic->SetIsLoopSound(true);
            c_streamingMusic->SetGain(0.1f);
            mAmbientMusicDummy->AddComponent(c_streamingMusic);
        }
    }
#endif

    if (mUiController) {
        mUiController->OnPostLevelInit();
    }
}

void TestFeaturesLevel::PostPlayLevelFinished()
{
    Base::PostPlayLevelFinished();
    if (mUiController) {
        mUiController->PostPlayLevelFinished();
    }

#ifdef DEBUG
    if (EngineUtility::EngineConfigHolder::GetInstance()->GetEngineConfig().EnableAmbientMusic) {
        mAmbientMusicDummy->GetComponentsByType<StreamingSoundComponent>().back()->PlayStream();
    }
#endif
}

void TestFeaturesLevel::InitLevel()
{
    Base::InitLevel();
    CreateScene();
    if (mUiController) {
        mUiController->OnLevelInit();
    }
}

void TestFeaturesLevel::UnloadLevel()
{
    mUiController->CleanUp();
    mUiController.reset();
}

void TestFeaturesLevel::Tick(const float deltaTimeSec)
{
    if (mUiController) {
        mUiController->Tick(deltaTimeSec);
    }

    if (mInputComponent) {
        const auto& mouseBindings = mInputComponent->GetMouseBindings();

        if (const auto& sceneSp = mSceneWp.lock()) {

            if (mouseBindings->GetKeyState(eMouseKeys::MouseButtonLeft) == KeyState::PRESSED) {
                const auto particlesActor = sceneSp->GetActorByName("ParticlesActor");
                assert(particlesActor);
                particlesActor->GetComponentsByType<ParticleSystemBaseComponent>().back()->EmitParticles();
            }
        }
    }
}

void TestFeaturesLevel::UnpausableTick(const float deltaTimeSec)
{
    if (mUiController) {
        mUiController->UnpausableTick(deltaTimeSec);
    }
}
} // namespace TestFeatures
