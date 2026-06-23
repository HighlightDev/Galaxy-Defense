#include "LuaScriptComponent.h"

#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptExecutors/LuaEngineScriptExecutor.h"
#include "Core/InterThreadCommunicationMgr.h"

using namespace EngineCore::Scripts;

namespace EngineCore {

eComponentType LuaScriptComponent::GetComponentType() const
{
    return eComponentType::SCRIPT_COMPONENT;
}

void LuaScriptComponent::Tick(const float deltaTimeSec, const float playSpeed)
{
}

void LuaScriptComponent::OnRegistered()
{
    Component::OnRegistered();

    if (const auto& sceneSp = m_sceneWP.lock()) {
        static constexpr uint64_t functionId = Hash64_CT("LuaScriptComponent::Initialize");
        mLuaScriptExecutor = std::make_shared<LuaEngineScriptExecutor>(mScriptName);
        sceneSp->GetInterThreadCommunicationManager().ExecuteOnLuaThread(
            eEnqueueJobPolicy::IF_DUPLICATE_NO_PUSH,
            0,
            functionId,
            [luaScriptExecutor = mLuaScriptExecutor](
                std::weak_ptr<Graphics::Renderer::SceneRenderer> sceneRendererWp,
                std::weak_ptr<EngineCore::Scene> sceneWp,
                std::weak_ptr<::EngineCore::Scripts::LuaScriptProcessor> luaProcessorWp) {
                if (const auto& luaScriptProcessorSp = luaProcessorWp.lock()) {
                    luaScriptProcessorSp->RegisterLuaScriptExecutor(luaScriptExecutor);
                    luaScriptExecutor->RunScript();
                }
            });
    }
}
} // namespace EngineCore
