#include "LuaScriptProcessor.h"
#include "LuaScriptExecutors/LuaScriptExecutorBase.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"
#include "Core/GameCore/LoggerExtension.h"

namespace EngineCore
{
    namespace Scripts
    {
        LuaScriptProcessor::LuaScriptProcessor(InterThreadCommunicationMgr &interThreadMgr)
            : m_interThreadMgr(interThreadMgr),
              mInputLuaProxy(),
              mLuaProxies()
        {
            Initialize();
        }

        void LuaScriptProcessor::Tick(const float deltaTime)
        {
            for (const auto &luaScriptExecutor : mLuaScriptExecutors)
            {
                luaScriptExecutor->OnUpdate(deltaTime);
            }
        }

        void LuaScriptProcessor::RegisterLuaScriptExecutor(const std::shared_ptr<::EngineCore::Scripts::LuaScriptExecutorBase> &luaExecutor)
        {
            assert(!std::any_of(mLuaScriptExecutors.begin(), mLuaScriptExecutors.end(), [&](const auto &scriptExecutor)
                                { return scriptExecutor->GetUId() == luaExecutor->GetUId(); }));
            mLuaScriptExecutors.emplace_back(luaExecutor);
            luaExecutor->SetLuaScriptProcessor(shared_from_this());
            luaExecutor->SetScene(m_interThreadMgr.GetSceneWP());
            luaExecutor->RegisterCallbacks();
        }

        void LuaScriptProcessor::UnregisterLuaScriptExecutor(const size_t uid)
        {
            assert(std::any_of(mLuaScriptExecutors.begin(), mLuaScriptExecutors.end(), [uid](const auto &scriptExecutor)
                               { return scriptExecutor->GetUId() == uid; }));

            mLuaScriptExecutors.erase(std::remove_if(mLuaScriptExecutors.begin(), mLuaScriptExecutors.end(), [uid](const auto &scriptExecutor)
                                                     { return scriptExecutor->GetUId() == uid; }));
        }

        std::shared_ptr<LuaScriptExecutorBase> LuaScriptProcessor::GetLuaScriptExecutor(const size_t uid) const
        {
            const auto foundIt = std::find_if(mLuaScriptExecutors.begin(), mLuaScriptExecutors.end(), [uid](const auto &scriptExecutor)
                                              { return scriptExecutor->GetUId() == uid; });
            return foundIt != mLuaScriptExecutors.end() ? *foundIt : nullptr;
        }

        std::shared_ptr<EngineInputLuaProxy> LuaScriptProcessor::GetEngineInputLuaProxy() const
        {
            return mInputLuaProxy;
        }

        std::shared_ptr<LuaProxy> LuaScriptProcessor::GetLuaProxy(const size_t luaProxyId) const
        {
            for (const auto luaProxy : mLuaProxies)
            {
                if (luaProxy && (luaProxy->GetLuaProxyId() == luaProxyId))
                {
                    return luaProxy;
                }
            }
            LogInfo("LuaScriptProcessor::GetLuaProxy => Error: luaProxy: ", luaProxyId, " doesn't exist");
            return nullptr;
        }

        InterThreadCommunicationMgr &LuaScriptProcessor::GetInterThreadCommunicationManager()
        {
            return m_interThreadMgr;
        }

        std::shared_ptr<OverlayManagerLuaProxy> LuaScriptProcessor::GetOverlayManagerLuaProxy() const
        {
            return mOverlayManagerLuaProxy;
        }

        void LuaScriptProcessor::SetOverlayManagerLuaProxy(const std::shared_ptr<OverlayManagerLuaProxy> &overlayManagerLuaProxy)
        {
            assert(overlayManagerLuaProxy);
            mOverlayManagerLuaProxy = overlayManagerLuaProxy;
        }

        void LuaScriptProcessor::AddLuaProxy(const std::shared_ptr<LuaProxy> &luaProxy)
        {
            assert(!GetLuaProxy(luaProxy->GetLuaProxyId()));
            mLuaProxies.emplace_back(luaProxy);
        }

        void LuaScriptProcessor::RemoveLuaProxy(const int32_t luaProxyId)
        {
            assert(GetLuaProxy(luaProxyId));
            mLuaProxies.erase(std::remove_if(mLuaProxies.begin(), mLuaProxies.end(), [luaProxyId](const auto& luaProxySp) {
                return luaProxySp->GetLuaProxyId() == luaProxyId;
            }));
        }

        void LuaScriptProcessor::Initialize()
        {
            mInputLuaProxy = std::make_shared<EngineInputLuaProxy>();
        }
    }
}