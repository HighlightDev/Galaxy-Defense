#include "LuaScriptProcessor.h"

#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/LoggerExtension.h"
#include "LuaScriptExecutors/LuaScriptExecutorBase.h"

namespace EngineCore {
namespace Scripts {
LuaScriptProcessor::LuaScriptProcessor(InterThreadCommunicationMgr& interThreadMgr)
    : m_interThreadMgr(interThreadMgr)
    , mInputLuaProxy()
    , mLuaProxies()
{
    Initialize();
}

void LuaScriptProcessor::Tick(const float deltaTimeSec, const float playSpeed)
{
    for (const auto& luaScriptExecutor : mLuaScriptExecutors) {
        if (luaScriptExecutor->IsEnabled()) {
            luaScriptExecutor->OnUpdate(deltaTimeSec);
        }
    }
}

void LuaScriptProcessor::RegisterLuaScriptExecutor(
    const std::shared_ptr<::EngineCore::Scripts::LuaScriptExecutorBase>& luaExecutor)
{
    ext_assert(
        !std::any_of(
            mLuaScriptExecutors.begin(),
            mLuaScriptExecutors.end(),
            [&](const auto& scriptExecutor) { return scriptExecutor->GetUId() == luaExecutor->GetUId(); }),
        "LuaScriptProcessor::RegisterLuaScriptExecutor: LuaScriptExecutor with the same UId already registered");
    mLuaScriptExecutors.emplace_back(luaExecutor);
    luaExecutor->SetLuaScriptProcessor(shared_from_this());
    luaExecutor->SetScene(m_interThreadMgr.GetSceneWP());
    luaExecutor->RegisterCallbacks();
}

void LuaScriptProcessor::UnregisterLuaScriptExecutor(const size_t uid)
{
    ext_assert(
        std::any_of(
            mLuaScriptExecutors.begin(),
            mLuaScriptExecutors.end(),
            [uid](const auto& scriptExecutor) { return scriptExecutor->GetUId() == uid; }),
        "LuaScriptProcessor::UnregisterLuaScriptExecutor: LuaScriptExecutor with provided UId not found");

    mLuaScriptExecutors.erase(
        std::remove_if(mLuaScriptExecutors.begin(), mLuaScriptExecutors.end(), [uid](const auto& scriptExecutor) {
            return scriptExecutor->GetUId() == uid;
        }));
}

std::shared_ptr<LuaScriptExecutorBase> LuaScriptProcessor::GetLuaScriptExecutor(const size_t uid) const
{
    const auto foundIt = std::find_if(mLuaScriptExecutors.begin(), mLuaScriptExecutors.end(), [uid](const auto& scriptExecutor) {
        return scriptExecutor->GetUId() == uid;
    });
    return foundIt != mLuaScriptExecutors.end() ? *foundIt : nullptr;
}

std::shared_ptr<EngineInputLuaProxy> LuaScriptProcessor::GetEngineInputLuaProxy() const
{
    return mInputLuaProxy;
}

std::shared_ptr<LuaProxy> LuaScriptProcessor::GetLuaProxy(const size_t luaProxyId) const
{
    for (const auto luaProxy : mLuaProxies) {
        if (luaProxy && (luaProxy->GetLuaProxyId() == luaProxyId)) {
            return luaProxy;
        }
    }
    return nullptr;
}

InterThreadCommunicationMgr& LuaScriptProcessor::GetInterThreadCommunicationManager()
{
    return m_interThreadMgr;
}

std::shared_ptr<OverlayManagerLuaProxy> LuaScriptProcessor::GetOverlayManagerLuaProxy() const
{
    return mOverlayManagerLuaProxy;
}

void LuaScriptProcessor::SetOverlayManagerLuaProxy(const std::shared_ptr<OverlayManagerLuaProxy>& overlayManagerLuaProxy)
{
    ext_assert(overlayManagerLuaProxy, "LuaScriptProcessor::SetOverlayManagerLuaProxy: overlayManagerLuaProxy is nullptr");
    mOverlayManagerLuaProxy = overlayManagerLuaProxy;
}

void LuaScriptProcessor::AddLuaProxy(const std::shared_ptr<LuaProxy>& luaProxy)
{
    ext_assert(
        !GetLuaProxy(luaProxy->GetLuaProxyId()),
        "LuaScriptProcessor::AddLuaProxy: LuaProxy with the same LuaProxyId already exists");
    mLuaProxies.emplace_back(luaProxy);
}

void LuaScriptProcessor::RemoveLuaProxy(const int32_t luaProxyId)
{
    const auto& luaProxySp = GetLuaProxy(luaProxyId);
    ext_assert(luaProxySp, "LuaScriptProcessor::RemoveLuaProxy: LuaProxy with provided LuaProxyId not found");
    luaProxySp->CleanUp();
    mLuaProxies.erase(std::remove_if(mLuaProxies.begin(), mLuaProxies.end(), [luaProxyId](const auto& luaProxySp) {
        return luaProxySp->GetLuaProxyId() == luaProxyId;
    }));
}

void LuaScriptProcessor::Initialize()
{
    mInputLuaProxy = std::make_shared<EngineInputLuaProxy>();
    mInputLuaProxy->Initialize();
}

void LuaScriptProcessor::CleanUp()
{
    mInputLuaProxy->CleanUp();
    mOverlayManagerLuaProxy->CleanUp();
    mOverlayManagerLuaProxy.reset();

    for (const auto& luaScriptExecutor : mLuaScriptExecutors) {
        luaScriptExecutor->CleanUp();
    }
    mLuaScriptExecutors.clear();

    for (const auto& luaProxy : mLuaProxies) {
        luaProxy->CleanUp();
    }
    mLuaProxies.clear();
}
} // namespace Scripts
} // namespace EngineCore