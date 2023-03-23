#include "UiCanvasLuaProxy.h"
#include "Core/GameCore/GUI/UiElements/UiCanvas.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/GameCore/Scene.h"

#include <json/json.hpp>

using namespace EngineCore;
using namespace EngineCore::GUI;

namespace EngineCore
{
    namespace Scripts
    {
        UiCanvasLuaProxy::UiCanvasLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiCanvas> &ownerCanvas)
            : LuaProxy(),
              mCanvasName(ownerCanvas->GetName()),
              mIsVisible(ownerCanvas->IsVisible())
        {
            mLuaProxyId = ownerCanvas->GetLuaProxyId();
            SetReplicatorId(ownerCanvas->GetReplicatorId());
        }

        std::string UiCanvasLuaProxy::GetCanvasName() const
        {
            return mCanvasName;
        }

        void UiCanvasLuaProxy::SetIsVisible_FromGameThread(const bool isVisible)
        {
            if (mIsVisible != isVisible)
            {
                mIsVisible = isVisible;
                mIsLuaDataDirty = true;
            }
        }

        void UiCanvasLuaProxy::OnLuaThreadDataUpdated(const std::string &jsonParameters)
        {
            static constexpr auto functionId = Hash64_CT("UiCanvasLuaProxy::OnLuaThreadDataUpdated");
            if (const auto sceneSp = mSceneWp.lock())
            {
                const auto replicatorId = GetReplicatorId();
                sceneSp->ExecuteOnGameThread(eEnqueueJobPolicy::IF_DUPLICATE_REPLACE, mLuaProxyId, functionId, [sceneSp, replicatorId, jsonStr = jsonParameters]() {
                    const auto &replicator = sceneSp->GetEngineToLuaReplicatorById(replicatorId);
                    assert(replicator);
                    const auto & canvas = std::static_pointer_cast<::EngineCore::GUI::UiCanvas>(replicator);
                    assert(canvas);
                    canvas->SyncFromLuaJsonProperties(jsonStr);
                });
            }
        }

        std::string UiCanvasLuaProxy::GetGameThreadData()
        {
            nlohmann::json jsonObj;
            jsonObj["name"] = mCanvasName;
            jsonObj["visible"] = mIsVisible;

            mIsLuaDataDirty = false;
            return jsonObj.dump();
        }

        bool UiCanvasLuaProxy::IsVisible() const
        {
            return mIsVisible;
        }
    }
}
