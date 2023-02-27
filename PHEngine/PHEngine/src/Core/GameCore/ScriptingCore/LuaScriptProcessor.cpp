#include "LuaScriptProcessor.h"
#include "LuaScriptExecutorBase.h"
#include "Core/GameCore/Components/InputComponent.h"
#include "Core/GameCore/Components/ComponentCreators/InputComponentCreator.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"

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
            luaExecutor->RunScript();
        }

        std::shared_ptr<EngineInputLuaProxy> LuaScriptProcessor::GetEngineInputLuaProxy() const
        {
            return mInputLuaProxy;
        }

        std::shared_ptr<LuaProxy> LuaScriptProcessor::GetLuaProxy(const size_t luaProxyId) const
        {
            const auto luaIt = std::find_if(mLuaProxies.begin(), mLuaProxies.end(), [=](const auto &luaProxy)
                                            { return luaProxyId == luaProxy->GetLuaProxyId(); });
            return luaIt != mLuaProxies.end() ? *luaIt : nullptr;
        }

        void LuaScriptProcessor::AddLuaProxy(const std::shared_ptr<LuaProxy> &luaProxy)
        {
            assert(!GetLuaProxy(luaProxy->GetLuaProxyId()));
            mLuaProxies.emplace_back(luaProxy);
        }

        void LuaScriptProcessor::Initialize()
        {
            if (const auto &sceneSp = m_interThreadMgr.GetSceneWP().lock())
            {
                mInputLuaProxy = std::make_shared<EngineInputLuaProxy>();
            }
        }
    }
}