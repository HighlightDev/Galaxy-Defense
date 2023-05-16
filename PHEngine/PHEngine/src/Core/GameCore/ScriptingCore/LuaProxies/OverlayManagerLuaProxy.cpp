#include "OverlayManagerLuaProxy.h"
#include "Core/GameCore/GUI/OverlayManagement/OverlayManager.h"
#include "Core/GameCore/Scene.h"
#include "Core/GameCore/ScriptingCore/LuaScriptProcessor.h"

using namespace EngineCore::GUI;
using namespace EngineCore;

namespace EngineCore
{
    namespace Scripts
    {
        OverlayManagerLuaProxy::OverlayManagerLuaProxy(const std::shared_ptr<OverlayManager> &owner)
            : LuaProxy(),
              mCurrentOverlayName(),
              mActiveBackgroundOverlays()
        {
            mLuaProxyId = CreateUniqueLuaProxyId();
            SetReplicatorId(owner->GetReplicatorId());
            owner->SetLuaProxyId(mLuaProxyId);
        }

        void OverlayManagerLuaProxy::SetCurrentOverlay(const std::string &currentOverlayName)
        {
            mCurrentOverlayName = currentOverlayName;
        }

        void OverlayManagerLuaProxy::SetActiveBackgroundOverlays(const std::unordered_set<std::string> &backgroundOverlays)
        {
            mActiveBackgroundOverlays = backgroundOverlays;
        }

        std::string OverlayManagerLuaProxy::GetCurrentOverlayName() const
        {
            return mCurrentOverlayName;
        }

        void OverlayManagerLuaProxy::OpenOverlay(const std::string &overlayName)
        {
            static constexpr auto functionId = Hash64_CT("OverlayManagerLuaProxy::OpenOverlay");
            if (const auto sceneSp = mSceneWp.lock())
            {
                const auto replicatorId = GetReplicatorId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, overlayName]()
                                                                                  {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & overlayManager = std::static_pointer_cast<OverlayManager>(replicator);
                    assert(overlayManager);
                    overlayManager->OpenOverlay(overlayName); });
            }
        }

        void OverlayManagerLuaProxy::OpenBackgroundOverlay(const std::string &overlayName)
        {
            static constexpr auto functionId = Hash64_CT("OverlayManagerLuaProxy::OpenBackgroundOverlay");
            if (const auto sceneSp = mSceneWp.lock())
            {
                const auto replicatorId = GetReplicatorId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, overlayName]()
                                                                                  {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & overlayManager = std::static_pointer_cast<OverlayManager>(replicator);
                    assert(overlayManager);
                    overlayManager->OpenBackgroundOverlay(overlayName); });
            }
        }

        void OverlayManagerLuaProxy::CloseCurrentOverlay()
        {
            static constexpr auto functionId = Hash64_CT("OverlayManagerLuaProxy::CloseCurrentOverlay");
            if (const auto sceneSp = mSceneWp.lock())
            {
                const auto replicatorId = GetReplicatorId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId]()
                                                                                  {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & overlayManager = std::static_pointer_cast<OverlayManager>(replicator);
                    assert(overlayManager);
                    overlayManager->CloseCurrentOverlay(); });
            }
        }

        void OverlayManagerLuaProxy::CloseBackgroundOverlay(const std::string &overlayName)
        {
            static constexpr auto functionId = Hash64_CT("OverlayManagerLuaProxy::CloseBackgroundOverlay");
            if (const auto sceneSp = mSceneWp.lock())
            {
                const auto replicatorId = GetReplicatorId();
                sceneSp->GetInterThreadCommunicationManager().ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, overlayName]()
                                                                                  {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & overlayManager = std::static_pointer_cast<OverlayManager>(replicator);
                    assert(overlayManager);
                    overlayManager->CloseBackgroundOverlay(overlayName); });
            }
        }

        void OverlayManagerLuaProxy::OnLuaThreadDataUpdated(const std::string &jsonParameters)
        {
        }

        std::string OverlayManagerLuaProxy::GetGameThreadData()
        {
            return "";
        }
    }
}
